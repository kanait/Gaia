

#ifndef _MESHFRAME_STATIC_TET_MESH_H_
#define _MESHFRAME_STATIC_TET_MESH_H_

#include "BaseTMesh.h"
#include "TetMeshTypeDefs.h"
#include "../Types/TypeDefs.h"

namespace MF
{
	namespace TetMesh
	{

		template <typename DType, typename TVertexType, typename VertexType, typename HalfEdgeType, typename TEdgeType, typename EdgeType, typename HalfFaceType, typename FaceType, typename TetType>
		class CTMeshStatic : public CTMeshBase<DType, TVertexType, VertexType, HalfEdgeType, TEdgeType, EdgeType, HalfFaceType, FaceType, TetType>
		{
		public:
                  using DataType = DType;  // ここで DType 型を追加
    typedef CTMeshStatic<DType, TVertexType, VertexType, HalfEdgeType, TEdgeType, EdgeType, HalfFaceType, FaceType, TetType>* Ptr;
    typedef std::shared_ptr<CTMeshStatic<DType, TVertexType, VertexType, HalfEdgeType, TEdgeType, EdgeType, HalfFaceType, FaceType, TetType>> SharedPtr;
		  //			typedef CTMeshStatic<DType, TVType, VType, HEType, TEType, EType, HFType, FType, TType>* Ptr;
		  //			typedef std::shared_ptr<CTMeshStatic<DType, TVType, VType, HEType, TEType, EType, HFType, FType, TType>> SharedPtr;

			/*!
				return all the vertex position as a 3xN matrix
			*/
			TVerticesMat<DType>& vertPos() { return mVertPos; };
			TTetIdsMat& tetVIds() { return mTetVIds; };

			TVec3Block<DType> vert(size_t vId) { return mVertPos.block<3, 1>(0, vId); }
			Vec4BlockI tet(size_t tId) { return mTetVIds.block<4, 1>(0, tId);  }

			/*! access the vertex with ID */
			virtual VertexType* idVertex(int id) { return &this->mVContainer[id]; };

			/*! access the tet with ID */
			virtual TetType* idTet(int id) { return &this->mTContainer[id]; };

			/*!
			Load tet mesh from a ".t" file
			*/
			void load_t(const char* input, bool checkOrientation = false);



		protected:
			TVerticesMat<DType> mVertPos;
			TTetIdsMat mTetVIds;

		};

		template <typename DType, typename TVertexType, typename VertexType, typename HalfEdgeType, typename TEdgeType, typename EdgeType, typename HalfFaceType, typename FaceType, typename TetType>
		void CTMeshStatic<DType, TVertexType, VertexType, HalfEdgeType, TEdgeType, EdgeType, HalfFaceType, FaceType, TetType>::load_t(const char* input, bool checkOrientation)
		{
			std::ostringstream oss;
			std::istringstream iss;

			this->addVProp(this->mVHFArrayHandle);
			this->addVProp(this->mVTEArrayHandle);

			this->m_maxVertexId = 0;

			std::fstream is(input, std::fstream::in);

			if (is.fail())
			{
				fprintf(stderr, "Error in opening file %s\n", input);
				return;
			}

			char buffer[MAX_LINE];

			this->m_nVertices = 0;
			this->m_nTets = 0;
			this->m_nEdges = 0;

			while (!is.eof())
			{
				is.getline(buffer, MAX_LINE);
				std::string line(buffer);
				line = strutilTetMesh::trim(line);
				strutilTetMesh::Tokenizer stokenizer(line, " \r\n");

				stokenizer.nextToken();
				std::string token = stokenizer.getToken();

				if (token == "Vertex") this->m_nVertices++;
				if (token == "Tet") this->m_nTets++;
				if (token == "Edge") this->m_nEdges++;
			}

			is.clear();              // forget we hit the end of file
			is.seekg(0, std::ios::beg);   // move to the start of the file

			mVertPos.resize(3, this->m_nVertices);
			mTetVIds.resize(4, this->m_nTets);

			//read in the vertices
			int vId = 0;
			std::map<int, int> tFileIdToVId;
			for (int i = 0; i < this->m_nVertices && is.getline(buffer, MAX_LINE); i++)
			{
				std::string line(buffer);
				line = strutilTetMesh::trim(line);
				strutilTetMesh::Tokenizer stokenizer(line, " \r\n");

				stokenizer.nextToken();
				std::string token = stokenizer.getToken();

				if (token != "Vertex")
				{
					fprintf(stderr, "File Format Error\r\n");
					return;
				}

				stokenizer.nextToken();
				token = stokenizer.getToken();
				int vIdTFile = strutilTetMesh::parseString<int>(token, iss);

				TVec3<DType> p;
				for (int k = 0; k < 3; k++)
				{
					stokenizer.nextToken();
					std::string token = stokenizer.getToken();
					p[k] = strutilTetMesh::parseString<float>(token, iss);
				}

				VertexType* v = this->createVertexWithIndex();
				v->id() = vId;
				v->setPVertPos(&mVertPos);
				v->position() = p;

				tFileIdToVId.insert({ vIdTFile, vId });
				++vId;
				if (!stokenizer.nextToken("\t\r\n")) continue;
				token = stokenizer.getToken();
			}

			//read in tets 
			int tid = 0;
			for (int id = 0; id < this->m_nTets && is.getline(buffer, MAX_LINE); id++)
			{
				int vIds[4];

				std::string line(buffer);
				line = strutilTetMesh::trim(line);
				strutilTetMesh::Tokenizer stokenizer(line, " \r\n");

				stokenizer.nextToken();
				std::string token = stokenizer.getToken();

				if (token != "Tet")
				{
					fprintf(stderr, "File Format Error\r\n");
					return;
				}

				//skip the first "4" in the line
				stokenizer.nextToken();
				token = stokenizer.getToken();
				// int tid = strutilTetMesh::parseString<int>(token, iss);


				for (int k = 0; k < 4; k++)
				{
					stokenizer.nextToken();
					std::string token = stokenizer.getToken();
					int vIdTFile = strutilTetMesh::parseString<int>(token, iss);
					vIds[k] = tFileIdToVId[vIdTFile];
				}

				TetType* pT = this->createTetWithIndex();
				pT->id() = tid;

				if (checkOrientation) {
					this->_construct_tet_orientation(pT, tid, vIds);
				}
				else {
					this->_construct_tet(pT, tid, vIds);
				}
				mTetVIds.block<4, 1>(0, tid) << vIds[0], vIds[1], vIds[2], vIds[3];
				tid++;

				if (!stokenizer.nextToken("\t\r\n")) continue;
		
			}

			this->_construct_faces();
			this->_construct_edges();

			for (int id = 0; id < this->m_nEdges && is.getline(buffer, MAX_LINE); id++)
			{
				std::string line(buffer);
				line = strutilTetMesh::trim(line);
				strutilTetMesh::Tokenizer stokenizer(line, " \r\n");

				stokenizer.nextToken();
				std::string token = stokenizer.getToken();

				if (token != "Edge")
				{
					fprintf(stderr, "File Format Error\r\n");
					return;
				}

				stokenizer.nextToken();
				token = stokenizer.getToken();
				int id1 = strutilTetMesh::parseString<int>(token, iss);

				stokenizer.nextToken();
				token = stokenizer.getToken();
				int id2 = strutilTetMesh::parseString<int>(token, iss);

				VertexType* pV1 = idVertex(id1);
				VertexType* pV2 = idVertex(id2);

				EdgeType* pE = this->VertexEdge(pV1, pV2);

				if (!stokenizer.nextToken("\t\r\n"))
				{
					continue;
				}

				token = stokenizer.getToken();
			}

			this->m_nEdges = (int)this->mEContainer.size();

			is.close();

			for (auto vIter = this->mVContainer.begin(); vIter != this->mVContainer.end(); vIter++)
			{
				VertexType* pV = *vIter;
				if (pV->id() > this->m_maxVertexId)
				{
					this->m_maxVertexId = pV->id();
				}
			}

			// label the boundary for faces and vertices
			for (auto fIter = this->mFContainer.begin(); fIter != this->mFContainer.end(); ++fIter)
			{
				FaceType* pF = *fIter;
				if (this->FaceLeftHalfFace(pF) == NULL || this->FaceRightHalfFace(pF) == NULL)
				{
					pF->boundary() = true;
					HalfFaceType* pH =
						this->FaceLeftHalfFace(pF) == NULL ? this->FaceRightHalfFace(pF) : this->FaceLeftHalfFace(pF);
					//added by Anka, mark edge as boundary
					HalfEdgeType* pHE = (HalfEdgeType*)pH->half_edge();

					for (int i = 0; i < 3; ++i)
					{
						EdgeType* pE = this->HalfEdgeEdge(pHE);
						int vid = pH->key(i);
						VertexType* v = idVertex(vid);
						v->boundary() = true;
						pE->boundary() = true;
						pHE = this->HalfEdgeNext(pHE);
					}
				}
			}

			for (auto vIter = this->mVContainer.begin(); vIter != this->mVContainer.end(); vIter++)
			{
				VertexType* pV = *vIter;
				pV->edges()->shrink_to_fit();
				pV->tvertices()->shrink_to_fit();
			}

			this->removeVProp(this->mVTEArrayHandle);

		}
	}
}

#endif
