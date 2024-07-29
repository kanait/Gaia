#pragma once
#include <iterator>
#include <algorithm>
#include <list>
#include <vector>

#include "BaseMesh.h"
#include "HalfEdge.h"

namespace MF {
	namespace TriMesh {
		template<typename DType, typename VertexType, typename EdgeType, typename FaceType, typename HalfEdgeType>
		struct CIteratorCore {
		public:
			typedef CMeshBase<DType, VertexType, EdgeType, FaceType, HalfEdgeType> MeshType;
			typedef MeshType* MeshPtr;
			typedef VertexType* VPtr;
			typedef EdgeType* EPtr;
			typedef FaceType* FPtr;
			typedef HalfEdgeType* HEPtr;

		public:
                  class VOutHEIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = HEPtr;
    using difference_type = std::ptrdiff_t;
    using pointer = HEPtr*;
    using reference = HEPtr&;

    VOutHEIterator(const VPtr& pV) : _pV(pV), _iter(pV->outHEs().begin()) {}
    VOutHEIterator(const VPtr& pV, CHalfEdge** iter) : _pV(pV), _iter(iter) {}

    VOutHEIterator& operator++() { ++_iter; return *this; }
    VOutHEIterator operator++(int) { VOutHEIterator tmp(_pV, _iter); ++_iter; return tmp; }

    bool operator==(const VOutHEIterator& otherIter) { return _iter == otherIter._iter; }
    bool operator!=(const VOutHEIterator& otherIter) { return _iter != otherIter._iter; }
    HEPtr operator*() { return (HEPtr)(*_iter); }
    HEPtr value() { return (HEPtr)(*_iter); }

    VOutHEIterator begin() { return VOutHEIterator(_pV); }
    VOutHEIterator end() { return VOutHEIterator(_pV, _pV->outHEs().end()); }

    HEPtr* get() { return _iter; }

private:
    CHalfEdge** _iter;
    VPtr _pV;
};

			/*
			* Iterator to access the out halfedge of a vertex in counter-clockwise direction.
			* Avalible only in manifold surface.
			* It is faster than the VoutHEIterator, so try to use this in manifold case.
			* \param pV the pointer to the vertex
			*/
                  class VCcwOutHEIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = HEPtr;
    using difference_type = std::ptrdiff_t;
    using pointer = HEPtr*;
    using reference = HEPtr&;

    VCcwOutHEIterator(const VPtr& pV) : _pV(pV), _pHE(MeshType::vertexMostClwOutHalfEdge(pV)) {}
    VCcwOutHEIterator(const VPtr& pV, const HEPtr& pHE) : _pV(pV), _pHE(pHE) {}

    VCcwOutHEIterator& operator++() {
        _pHE = _pHE == MeshType::vertexMostCcwOutHalfEdge(_pV) ? NULL : MeshType::vertexNextCcwOutHalfEdge(_pHE);
        return *this;
    }
    VCcwOutHEIterator operator++(int) {
        VCcwOutHEIterator tmp = *this;
        _pHE = _pHE == MeshType::vertexMostCcwOutHalfEdge(_pV) ? NULL : MeshType::vertexNextCcwOutHalfEdge(_pHE);
        return tmp;
    }

    bool operator==(const VCcwOutHEIterator& otherIter) { return _pHE == otherIter._pHE; }
    bool operator!=(const VCcwOutHEIterator& otherIter) { return _pHE != otherIter._pHE; }
    HEPtr operator*() { return _pHE; }
    HEPtr value() { return _pHE; }

    VCcwOutHEIterator begin() { return VCcwOutHEIterator(_pV); }
    VCcwOutHEIterator end() { return VCcwOutHEIterator(_pV, NULL); } //why null???may be a he ?

    HEPtr get() { return _pHE; }

private:
    VPtr _pV;
    HEPtr _pHE;
};

			/*
			* Iterator to access the out halfedge of a vertex in clockwise direction.
			* Avalible only in manifold surface.
			* It is faster than the VoutHEIterator, so try to use this in manifold case.
			* \param pV the pointer to the vertex
			*/
                  class VClwInHEIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = HEPtr;
    using difference_type = std::ptrdiff_t;
    using pointer = HEPtr*;
    using reference = HEPtr&;

    VClwInHEIterator(const VPtr& pV) : _pV(pV), _pHE(MeshType::vertexMostCcwInHalfEdge(pV)) {}
    VClwInHEIterator(const VPtr& pV, const HEPtr& pHE) : _pV(pV), _pHE(pHE) {}

    /*! iterator ++ */
    VClwInHEIterator& operator++() {
        _pHE = _pHE == MeshType::vertexMostClwInHalfEdge(_pV) ? _pHE = NULL : MeshType::vertexNextClwInHalfEdge(_pHE);
        return *this;
    }

    /*! ++ iterator */
    VClwInHEIterator operator++(int) {
        VClwInHEIterator tmp(_pV, _pHE);
        _pHE = _pHE == MeshType::vertexMostClwInHalfEdge(_pV) ? _pHE = NULL : MeshType::vertexNextClwInHalfEdge(_pHE);
        return tmp;
    }

    /*! bool operator */
    bool operator==(const VClwInHEIterator& otherIter) { return _pHE == otherIter._pHE; }
    bool operator!=(const VClwInHEIterator& otherIter) { return _pHE != otherIter._pHE; }

    /*! dereferencing */
    HEPtr operator*() { return _pHE; }
    HEPtr value() { return _pHE; }

    /*! return the begin and end iterators */
    VClwInHEIterator begin() { return VClwInHEIterator(_pV); }
    VClwInHEIterator end() { return VClwInHEIterator(_pV, NULL); }

    /*! get method */
    HEPtr get() { return _pHE; }

private:
    VPtr _pV;
    HEPtr _pHE;
};

                  class VCcwFIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = FPtr;
    using difference_type = std::ptrdiff_t;
    using pointer = FPtr*;
    using reference = FPtr&;

    VCcwFIterator(const VPtr& pV) : _pV(pV), _pHE(MeshType::vertexMostClwOutHalfEdge(pV)) {}

    /*! iterator ++ */
    VCcwFIterator& operator++() {
        _pHE = _pHE == MeshType::vertexMostCcwOutHalfEdge(_pV) ? NULL : MeshType::vertexNextCcwOutHalfEdge(_pHE);
        return *this;
    }

    /*! ++ iterator */
    VCcwFIterator operator++(int) {
        VCcwFIterator tmp = *this;
        _pHE = _pHE == MeshType::vertexMostCcwOutHalfEdge(_pV) ? NULL : MeshType::vertexNextCcwOutHalfEdge(_pHE);
        return tmp;
    }

    /*! bool operator */
    bool operator==(const VCcwFIterator& otherIter) { return _pHE == otherIter._pHE; }
    bool operator!=(const VCcwFIterator& otherIter) { return _pHE != otherIter._pHE; }

    /*! dereferencing */
    FPtr operator*() { return halfedgeFace(_pHE); }
    FPtr value() { return halfedgeFace(_pHE); }

    /*! return the begin and end iterators */
    VCcwFIterator begin() { return VCcwFIterator(_pV); }
    VCcwFIterator end() { return VCcwFIterator(_pV, NULL); } //why null???may be a he ?

    /*! get method */
    HEPtr get() { return _pHE; }

private:
    VCcwFIterator(const VPtr& pV, const HEPtr& pHE) : _pV(pV), _pHE(pHE) {}

    VPtr _pV;
    HEPtr _pHE;
};

                  class VCcwEIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = EPtr;
    using difference_type = std::ptrdiff_t;
    using pointer = EPtr*;
    using reference = EPtr&;

    VCcwEIterator(const VPtr& pV) : _pV(pV), _pHE(MeshType::vertexMostClwOutHalfEdge(pV)) {}

    /*! iterator ++ */
    VCcwEIterator& operator++() {
        if (MeshType::isBoundary(_pV)) {
            if (_pHE == MeshType::vertexMostCcwOutHalfEdge(_pV)) {
                _pHE = (HEPtr)_pHE->he_prev();
                reachBoundary = true;
            } else if (reachBoundary) {
                _pHE = NULL;
            } else {
                _pHE = MeshType::vertexNextCcwOutHalfEdge(_pHE);
            }
        } else {
            _pHE = _pHE == MeshType::vertexMostCcwOutHalfEdge(_pV) ? NULL : MeshType::vertexNextCcwOutHalfEdge(_pHE);
        }
        return *this;
    }

    /*! ++ iterator */
    VCcwEIterator operator++(int) {
        VCcwEIterator tmp(_pV, _pHE);
        if (MeshType::isBoundary(_pV)) {
            if (_pHE == MeshType::vertexMostCcwInHalfEdge(_pV)) {
                _pHE = (HEPtr)_pHE->he_prev();
                reachBoundary = true;
            } else if (reachBoundary) {
                _pHE = NULL;
            } else {
                _pHE = MeshType::vertexNextCcwInHalfEdge(_pHE);
            }
        } else {
            _pHE = _pHE == MeshType::vertexMostCcwInHalfEdge(_pV) ? NULL : MeshType::vertexNextClwInHalfEdge(_pHE);
        }
        return tmp;
    }

    /*! bool operator */
    bool operator==(const VCcwEIterator& otherIter) { return _pHE == otherIter._pHE; }
    bool operator!=(const VCcwEIterator& otherIter) { return _pHE != otherIter._pHE; }

    /*! dereferencing */
    EPtr operator*() { return MeshType::halfedgeEdge(_pHE); }
    EPtr value() { return MeshType::halfedgeEdge(_pHE); }

    /*! return the begin and end iterators */
    VCcwEIterator begin() { return VCcwEIterator(_pV); }
    VCcwEIterator end() { return VCcwEIterator(_pV, NULL); }

    /*! get method */
    HEPtr get() { return _pHE; }

private:
    VCcwEIterator(const VPtr& pV, const HEPtr& pHE) : _pV(pV), _pHE(pHE) {}

    VPtr _pV;
    HEPtr _pHE;
    bool reachBoundary = false;
};


                  class VClwEIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = EPtr;
    using difference_type = std::ptrdiff_t;
    using pointer = EPtr*;
    using reference = EPtr&;

    VClwEIterator(const VPtr& pV) : _pV(pV), _pHE(MeshType::vertexMostCcwInHalfEdge(pV)) {}

    /*! iterator ++ */
    VClwEIterator& operator++() {
        if (MeshType::isBoundary(_pV)) {
            if (_pHE == MeshType::vertexMostClwInHalfEdge(_pV)) {
                _pHE = (HEPtr)_pHE->he_next();
                reachBoundary = true;
            } else if (reachBoundary) {
                _pHE = NULL;
            } else {
                _pHE = MeshType::vertexNextClwInHalfEdge(_pHE);
            }
        } else {
            _pHE = _pHE == MeshType::vertexMostClwInHalfEdge(_pV) ? NULL : MeshType::vertexNextClwInHalfEdge(_pHE);
        }
        return *this;
    }

    /*! ++ iterator */
    VClwEIterator operator++(int) {
        VClwEIterator tmp(_pV, _pHE);
        if (MeshType::isBoundary(_pV)) {
            if (_pHE == MeshType::vertexMostClwInHalfEdge(_pV)) {
                _pHE = (HEPtr)_pHE->he_next();
                reachBoundary = true;
            } else if (reachBoundary) {
                _pHE = NULL;
            } else {
                _pHE = MeshType::vertexNextClwInHalfEdge(_pHE);
            }
        } else {
            _pHE = _pHE == MeshType::vertexMostClwInHalfEdge(_pV) ? NULL : MeshType::vertexNextClwInHalfEdge(_pHE);
        }
        return tmp;
    }

    /*! bool operator */
    bool operator==(const VClwEIterator& otherIter) { return _pHE == otherIter._pHE; }
    bool operator!=(const VClwEIterator& otherIter) { return _pHE != otherIter._pHE; }

    /*! dereferencing */
    EPtr operator*() { return MeshType::halfedgeEdge(_pHE); }
    EPtr value() { return MeshType::halfedgeEdge(_pHE); }

    /*! return the begin and end iterators */
    VClwEIterator begin() { return VClwEIterator(_pV); }
    VClwEIterator end() { return VClwEIterator(_pV, NULL); }

    /*! get method */
    HEPtr get() { return _pHE; }

private:
    VClwEIterator(const VPtr& pV, const HEPtr& pHE) : _pV(pV), _pHE(pHE) {}

    VPtr _pV;
    HEPtr _pHE;
    bool reachBoundary = false;
};

                  class VVIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = VPtr;
    using difference_type = std::ptrdiff_t;
    using pointer = VPtr*;
    using reference = VPtr&;

    VVIterator(VPtr pV) : _pV(pV), _iter(pV->outHEs().begin()) {}

    /*! iterator ++ */
    VVIterator& operator++() {
        if (MeshType::isBoundary(_pV)) {
            if (_pLastV != NULL) {
                ++_iter;
                _pLastV = NULL;
            } else {
                CHalfEdge* pLastHE = (*_iter)->he_prev();
                if (pLastHE->he_sym() != NULL) {
                    ++_iter;
                } else {
                    _pLastV = MeshType::halfedgeSource((HEPtr)pLastHE);
                }
            }
        } else {
            ++_iter;
        }
        return *this;
    }

    /*! ++ iterator */
    VVIterator operator++(int) {
        VVIterator tmp(_pV, _iter);
        if (MeshType::isBoundary(_pV)) {
            if (_pLastV != NULL) {
                ++_iter;
                _pLastV = NULL;
            } else {
                CHalfEdge* pLastHE = (*_iter)->he_prev();
                if (pLastHE->he_sym() != NULL) {
                    ++_iter;
                } else {
                    _pLastV = MeshType::halfedgeSource((HEPtr)pLastHE);
                }
            }
        } else {
            ++_iter;
        }
        return tmp;
    }

    /*! bool operator */
    bool operator==(const VVIterator& otherIter) { return _iter == otherIter._iter; }
    bool operator!=(const VVIterator& otherIter) { return _iter != otherIter._iter; }

    /*! dereferencing */
    VPtr operator*() {
        if (_pLastV == NULL)
            return (VPtr)(*_iter)->target();
        else
            return _pLastV;
    }

    VPtr value() {
        if (_pLastV == NULL)
            return (VPtr)(*_iter)->target();
        else
            return _pLastV;
    }

    /*! return the begin and end iterators */
    VVIterator begin() { return VVIterator(_pV); }
    VVIterator end() { return VVIterator(_pV, _pV->outHEs().end()); }

private:
    VVIterator(VPtr pV, CHalfEdge** iter) : _pV(pV), _iter(iter) {}

    CHalfEdge** _iter;
    VPtr _pV;
    VPtr _pLastV = NULL;
};

                  class VCcwVIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = VPtr;
    using difference_type = std::ptrdiff_t;
    using pointer = VPtr*;
    using reference = VPtr&;

    VCcwVIterator(const VPtr& pV) : _pV(pV), _pHE(MeshType::vertexMostClwOutHalfEdge(pV)) {}

    /*! iterator ++ */
    VCcwVIterator& operator++() {
        if (MeshType::isBoundary(_pV)) {
            if (_pHE == MeshType::vertexMostCcwOutHalfEdge(_pV)) {
                _pHE = (HEPtr)_pHE->he_prev();
                reachBoundary = true;
            } else if (reachBoundary) {
                _pHE = NULL;
            } else {
                _pHE = MeshType::vertexNextCcwOutHalfEdge(_pHE);
            }
        } else {
            _pHE = _pHE == MeshType::vertexMostCcwOutHalfEdge(_pV) ? NULL : MeshType::vertexNextCcwOutHalfEdge(_pHE);
        }
        return *this;
    }

    /*! ++ iterator */
    VCcwVIterator operator++(int) {
        VCcwVIterator tmp(_pV, _pHE);
        if (MeshType::isBoundary(_pV)) {
            if (_pHE == MeshType::vertexMostCcwInHalfEdge(_pV)) {
                _pHE = (HEPtr)_pHE->he_prev();
                reachBoundary = true;
            } else if (reachBoundary) {
                _pHE = NULL;
            } else {
                _pHE = MeshType::vertexNextCcwInHalfEdge(_pHE);
            }
        } else {
            _pHE = _pHE == MeshType::vertexMostCcwInHalfEdge(_pV) ? NULL : MeshType::vertexNextClwInHalfEdge(_pHE);
        }
        return tmp;
    }

    /*! bool operator */
    bool operator==(const VCcwVIterator& otherIter) { return _pHE == otherIter._pHE; }
    bool operator!=(const VCcwVIterator& otherIter) { return _pHE != otherIter._pHE; }

    /*! dereferencing */
    VPtr operator*() {
        if (reachBoundary)
            return MeshType::halfedgeSource(_pHE);
        else
            return MeshType::halfedgeTarget(_pHE);
    }

    VPtr value() {
        if (reachBoundary)
            return MeshType::halfedgeSource(_pHE);
        else
            return MeshType::halfedgeTarget(_pHE);
    }

    /*! return the begin and end iterators */
    VCcwVIterator begin() { return VCcwVIterator(_pV); }
    VCcwVIterator end() { return VCcwVIterator(_pV, NULL); }

private:
    VCcwVIterator(const VPtr& pV, const HEPtr& pHE) : _pV(pV), _pHE(pHE) {}

    VPtr _pV;
    HEPtr _pHE;
    bool reachBoundary = false;
};

                  class VClwVIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = VPtr;
    using difference_type = std::ptrdiff_t;
    using pointer = VPtr*;
    using reference = VPtr&;

    VClwVIterator(const VPtr& pV) : _pV(pV), _pHE(MeshType::vertexMostCcwInHalfEdge(pV)) {}

    /*! iterator ++ */
    VClwVIterator& operator++() {
        if (MeshType::isBoundary(_pV)) {
            if (_pHE == MeshType::vertexMostClwInHalfEdge(_pV)) {
                _pHE = (HEPtr)_pHE->he_next();
                reachBoundary = true;
            } else if (reachBoundary) {
                _pHE = NULL;
            } else {
                _pHE = MeshType::vertexNextClwInHalfEdge(_pHE);
            }
        } else {
            _pHE = _pHE == MeshType::vertexMostClwInHalfEdge(_pV) ? NULL : MeshType::vertexNextClwInHalfEdge(_pHE);
        }
        return *this;
    }

    /*! ++ iterator */
    VClwVIterator operator++(int) {
        VClwVIterator tmp(_pV, _pHE);
        if (MeshType::isBoundary(_pV)) {
            if (_pHE == MeshType::vertexMostClwInHalfEdge(_pV)) {
                _pHE = (HEPtr)_pHE->he_next();
                reachBoundary = true;
            } else if (reachBoundary) {
                _pHE = NULL;
            } else {
                _pHE = MeshType::vertexNextCcwInHalfEdge(_pHE);
            }
        } else {
            _pHE = _pHE == MeshType::vertexMostClwInHalfEdge(_pV) ? NULL : MeshType::vertexNextClwInHalfEdge(_pHE);
        }
        return tmp;
    }

    /*! bool operator */
    bool operator==(const VClwVIterator& otherIter) { return _pHE == otherIter._pHE; }
    bool operator!=(const VClwVIterator& otherIter) { return _pHE != otherIter._pHE; }

    /*! dereferencing */
    VPtr operator*() {
        if (reachBoundary)
            return MeshType::halfedgeTarget(_pHE);
        else
            return MeshType::halfedgeSource(_pHE);
    }

    VPtr value() {
        if (reachBoundary)
            return MeshType::halfedgeTarget(_pHE);
        else
            return MeshType::halfedgeSource(_pHE);
    }

    /*! return the begin and end iterators */
    VClwVIterator begin() { return VClwVIterator(_pV); }
    VClwVIterator end() { return VClwVIterator(_pV, NULL); }

    /*! get method */
    HEPtr get() { return _pHE; }

private:
    VClwVIterator(const VPtr& pV, const HEPtr& pHE) : _pV(pV), _pHE(pHE) {}

    VPtr _pV;
    HEPtr _pHE;
    bool reachBoundary = false;
};

                  class VEIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = EPtr;
    using difference_type = std::ptrdiff_t;
    using pointer = EPtr*;
    using reference = EPtr&;

    VEIterator(VPtr pV) : _pV(pV), _iter(pV->outHEs().begin()) {}

    /*! iterator ++ */
    VEIterator& operator++() {
        if (MeshType::isBoundary(_pV)) {
            if (_pLastE != NULL) {
                ++_iter;
                _pLastE = NULL;
            } else {
                CHalfEdge* pLastHE = (*_iter)->he_prev();
                if (pLastHE->he_sym() != NULL) {
                    ++_iter;
                } else {
                    _pLastE = MeshType::halfedgeEdge((HEPtr)pLastHE);
                }
            }
        } else {
            ++_iter;
        }
        return *this;
    }

    /*! ++ iterator */
    VEIterator operator++(int) {
        VEIterator tmp(_pV, _iter);
        if (MeshType::isBoundary(_pV)) {
            if (_pLastE != NULL) {
                ++_iter;
                _pLastE = NULL;
            } else {
                CHalfEdge* pLastHE = (*_iter)->he_prev();
                if (pLastHE->he_sym() != NULL) {
                    ++_iter;
                } else {
                    _pLastE = MeshType::halfedgeEdge((HEPtr)pLastHE);
                }
            }
        } else {
            ++_iter;
        }
        return tmp;
    }

    /*! bool operator */
    bool operator==(const VEIterator& otherIter) { return _iter == otherIter._iter; }
    bool operator!=(const VEIterator& otherIter) { return _iter != otherIter._iter; }

    /*! dereferencing */
    EPtr operator*() {
        if (_pLastE == NULL)
            return MeshType::halfedgeEdge((HEPtr)*_iter);
        else
            return _pLastE;
    }

    EPtr value() {
        if (_pLastE == NULL)
            return MeshType::halfedgeEdge((HEPtr)*_iter);
        else
            return _pLastE;
    }

    /*! return the begin and end iterators */
    VEIterator begin() { return VEIterator(_pV); }
    VEIterator end() { return VEIterator(_pV, _pV->outHEs().end()); }

private:
    VEIterator(VPtr pV, CHalfEdge** iter) : _pV(pV), _iter(iter) {}

    CHalfEdge** _iter;
    VPtr _pV;
    EPtr _pLastE = NULL;
};

                  class VFIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = FPtr;
    using difference_type = std::ptrdiff_t;
    using pointer = FPtr*;
    using reference = FPtr&;

    VFIterator(VPtr pV) : _pV(pV), _iter(pV->outHEs().begin()) {}

    /*! iterator ++ */
    VFIterator& operator++() { ++_iter; return *this; }

    /*! ++ iterator */
    VFIterator operator++(int) {
        VFIterator tmp(_pV, _iter);
        ++_iter;
        return tmp;
    }

    /*! bool operator */
    bool operator==(const VFIterator& otherIter) { return _iter == otherIter._iter; }
    bool operator!=(const VFIterator& otherIter) { return _iter != otherIter._iter; }

    /*! dereferencing */
    FPtr operator*() { return (FPtr)(*_iter)->face(); }
    FPtr value() { return (FPtr)(*_iter)->face(); }

    /*! return the begin and end iterators */
    VFIterator begin() { return VFIterator(_pV); }
    VFIterator end() { return VFIterator(_pV, _pV->outHEs().end()); }

    /*! get method */
    HEPtr* get() { return _iter; }

private:
    VFIterator(VPtr pV, CHalfEdge** iter) : _pV(pV), _iter(iter) {}

    CHalfEdge** _iter;
    VPtr _pV;
};

			// bad orgnized
                  class FHEIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = HEPtr;
    using difference_type = std::ptrdiff_t;
    using pointer = HEPtr*;
    using reference = HEPtr&;

    FHEIterator(FPtr pF) : _pF(pF), _pHE((HEPtr)pF->halfedge()) {}

    /*! iterator ++ */
    FHEIterator& operator++() {
        _pHE = (HEPtr)_pHE->he_next();
        if (_pHE == (HEPtr)_pF->halfedge()) _pHE = NULL;
        return *this;
    }

    /*! ++ iterator */
    FHEIterator operator++(int) {
        FHEIterator tmp(_pF, _pHE);
        ++_pHE;
        return tmp;
    }

    /*! bool operator */
    bool operator==(const FHEIterator& otherIter) { return _pHE == otherIter._pHE; }
    bool operator!=(const FHEIterator& otherIter) { return _pHE != otherIter._pHE; }

    /*! dereferencing */
    HEPtr operator*() { return _pHE; }
    HEPtr value() { return _pHE; }

    /*! return the begin and end iterators */
    FHEIterator begin() { return FHEIterator(_pF); }
    FHEIterator end() { return FHEIterator(_pF, NULL); }
    // FHEIterator end() { return FHIterator(_pF, (HEPtr)pF->halfedge()->he_prev())); }

    /*! get method */
    HEPtr* get() { return _pHE; }

private:
    FHEIterator(FPtr pF, HEPtr iter) : _pF(pF), _pHE(iter) {}

    HEPtr _pHE;
    FPtr _pF;
};

			//class FEIterator
                  class FEIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = EPtr;
    using difference_type = std::ptrdiff_t;
    using pointer = EPtr*;
    using reference = EPtr&;

    FEIterator(FPtr pF) : _pF(pF), _pHE((HEPtr)pF->halfedge()) {}
    FEIterator(FPtr pF, HEPtr iter) : _pF(pF), _pHE(iter) {}

    /*! iterator ++ */
    FEIterator& operator++() {
        _pHE = (HEPtr)_pHE->he_next();
        if (_pHE == (HEPtr)_pF->halfedge()) _pHE = NULL;
        return *this;
    }

    /*! ++ iterator */
    FEIterator operator++(int) {
        FEIterator tmp(_pF, _pHE);
        ++_pHE;
        return tmp;
    }

    /*! bool operator */
    bool operator==(const FEIterator& otherIter) { return _pHE == otherIter._pHE; }
    bool operator!=(const FEIterator& otherIter) { return _pHE != otherIter._pHE; }

    /*! dereferencing */
    EPtr operator*() { return (EPtr)_pHE->edge(); }
    EPtr value() { return (EPtr)_pHE->edge(); }

    /*! return the begin and end iterators */
    FEIterator begin() { return FEIterator(_pF); }
    FEIterator end() { return FEIterator(_pF, NULL); }
    // FEIterator end() { return FEIterator(_pF, (HEPtr)pF->halfedge()->he_prev()); }

    /*! get method */
    HEPtr* get() { return _pHE; }

private:
    HEPtr _pHE;
    FPtr _pF;
};

			//class FVItertor
                  class FVIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = VPtr;
    using difference_type = std::ptrdiff_t;
    using pointer = VPtr*;
    using reference = VPtr&;

    FVIterator(const FPtr& pF) : _pF(pF), _pHE(MeshType::faceHalfedge(pF)) {}

    /*! iterator ++ */
    FVIterator& operator++() {
        _pHE = (HEPtr)_pHE->he_next();
        if (_pHE == (HEPtr)_pF->halfedge()) _pHE = NULL;
        return *this;
    }

    /*! ++ iterator */
    FVIterator operator++(int) {
        FVIterator tmp(_pF, _pHE);
        ++_pHE;
        return tmp;
    }

    /*! bool operator */
    bool operator==(const FVIterator& otherIter) { return _pHE == otherIter._pHE; }
    bool operator!=(const FVIterator& otherIter) { return _pHE != otherIter._pHE; }

    /*! dereferencing */
    VPtr operator*() { return (VPtr)_pHE->vertex(); }
    VPtr value() { return (VPtr)_pHE->vertex(); }

    /*! return the begin and end iterators */
    FVIterator begin() { return FVIterator(_pF); }
    FVIterator end() { return FVIterator(_pF, NULL); }

    /*! get method */
    HEPtr get() { return _pHE; }

private:
    FVIterator(const FPtr& pF, const HEPtr& iter) : _pF(pF), _pHE(iter) {}

    HEPtr _pHE;
    FPtr _pF;
};

			//we need pmesh
                  class MVIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = VPtr;
    using difference_type = std::ptrdiff_t;
    using pointer = VPtr*;
    using reference = VPtr&;

    MVIterator(MeshPtr pM) : _pM(pM), _iter(pM->vertices().begin()) {}

    /*! iterator ++ */
    MVIterator& operator++() { ++_iter; return *this; }

    /*! ++ iterator */
    MVIterator operator++(int) {
        MVIterator tmp(_pM, _iter);
        ++_iter;
        return tmp;
    }

    /*! bool operator */
    bool operator==(const MVIterator& otherIter) { return _iter == otherIter._iter; }
    bool operator!=(const MVIterator& otherIter) { return _iter != otherIter._iter; }

    /*! dereferencing */
    VPtr operator*() { return *_iter; }
    VPtr value() { return *_iter; }

    /*! return the begin and end iterators */
    MVIterator begin() { return MVIterator(_pM); }
    MVIterator end() { return MVIterator(_pM, _pM->vertices().end()); }

private:
    MVIterator(MeshPtr pM, MPIterator<VertexType> iter) : _pM(pM), _iter(iter) {}

    MPIterator<VertexType> _iter;
    MeshPtr _pM;
};

                  class MFIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = FPtr;
    using difference_type = std::ptrdiff_t;
    using pointer = FPtr*;
    using reference = FPtr&;

    MFIterator(MeshPtr pM) : _pM(pM), _iter(pM->faces().begin()) {}

    /*! iterator ++ */
    MFIterator& operator++() { ++_iter; return *this; }

    /*! ++ iterator */
    MFIterator operator++(int) {
        MFIterator tmp(_pM, _iter);
        ++_iter;
        return tmp;
    }

    /*! bool operator */
    bool operator==(const MFIterator& otherIter) { return _iter == otherIter._iter; }
    bool operator!=(const MFIterator& otherIter) { return _iter != otherIter._iter; }

    /*! dereferencing */
    FPtr operator*() { return *_iter; }
    FPtr value() { return *_iter; }

    /*! return the begin and end iterators */
    MFIterator begin() { return MFIterator(_pM); }
    MFIterator end() { return MFIterator(_pM, _pM->faces().end()); }

private:
    MFIterator(MeshPtr pM, MPIterator<FaceType> iter) : _pM(pM), _iter(iter) {}

    MPIterator<FaceType> _iter;
    MeshPtr _pM;
};

                  class MEIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = EPtr;
    using difference_type = std::ptrdiff_t;
    using pointer = EPtr*;
    using reference = EPtr&;

    MEIterator(MeshPtr pM) : _pM(pM), _iter(pM->edges().begin()) {}

    /*! iterator ++ */
    MEIterator& operator++() { ++_iter; return *this; }

    /*! ++ iterator */
    MEIterator operator++(int) {
        MEIterator tmp(_pM, _iter);
        ++_iter;
        return tmp;
    }

    /*! bool operator */
    bool operator==(const MEIterator& otherIter) { return _iter == otherIter._iter; }
    bool operator!=(const MEIterator& otherIter) { return _iter != otherIter._iter; }

    /*! dereferencing */
    EPtr operator*() { return *_iter; }
    EPtr value() { return *_iter; }

    /*! return the begin and end iterators */
    MEIterator begin() { return MEIterator(_pM); }
    MEIterator end() {
        auto endIter = _pM->edges().end();
        return MEIterator(_pM, endIter);
    }

private:
    MEIterator(MeshPtr pM, MPIterator<EdgeType> iter) : _pM(pM), _iter(iter) {}

    MPIterator<EdgeType> _iter;
    MeshPtr _pM;
};

			//class MHEIterator

		};

		template<typename MeshType>
		struct CIterators : public CIteratorCore<typename MeshType::DataType, typename MeshType::VType, typename MeshType::EType, typename MeshType::FType, typename MeshType::HEType>
		{};
	}
}
