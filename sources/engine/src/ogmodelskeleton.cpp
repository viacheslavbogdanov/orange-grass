/*
 * OrangeGrass
 * Copyright (C) 2009 Vyacheslav Bogdanov.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/lgpl-3.0-standalone.html>.
 */
#include "OrangeGrass.h"
#include "ogmodelskeleton.h"


COGModelSkeleton::COGModelSkeleton()
    : m_NumFrames (0)
{
}


COGModelSkeleton::~COGModelSkeleton()
{
    Clear();
}


void COGModelSkeleton::Clear()
{
    m_NumFrames = 0;
    for (auto m = m_Nodes.begin(); m != m_Nodes.end(); ++m) { OG_SAFE_DELETE((*m)); }
    m_Nodes.clear();
}


void COGModelSkeleton::SetNumFrames(unsigned int _NumFrames)
{
    m_NumFrames = _NumFrames;
}


OGModelNode* COGModelSkeleton::AddNode(int _Idx, int _IdxParent, const float* _pfAnimMatrix)
{
    OGModelNode* pNode = new OGModelNode();
    pNode->nIdxParent = _IdxParent;
    pNode->nIdx = _Idx;
    const OGMatrix* pSrcMatrixArr = (const OGMatrix*)_pfAnimMatrix;
    if (m_NumFrames > 0)
    {
        pNode->mTransformList.reserve(m_NumFrames);
        for (unsigned int i = 0; i < m_NumFrames; ++i)
        {
            OGMatrix m;
            memcpy(m.f, &pSrcMatrixArr[i], sizeof(float) * 16);
            pNode->mTransformList.push_back(m);
        }
    }
    else
    {
        OGMatrix m;
        memcpy(m.f, pSrcMatrixArr, sizeof(float) * 16);
        pNode->mTransformList.push_back(m);
    }
    m_Nodes.push_back(pNode);
    return pNode;
}


OGModelNode* COGModelSkeleton::BuildSG ()
{
    bool bRootCreated = false;
    OGModelNode* pRoot = NULL;
    size_t n = m_Nodes.size();
    for (size_t i = 0; i < n; ++i)
    {
        OGModelNode* pCurNode = m_Nodes[i];
        pCurNode->nIdx = i;
        int curParent = pCurNode->nIdxParent;
        if (curParent == -1)
        {
            if (pRoot != NULL)
            {
                if (bRootCreated)
                {
                    AttachChild(pRoot, pCurNode);
                }
                else
                {
                    OGModelNode* pSingleRootNode = new OGModelNode();
                    OGMatrix mIdentity;
                    MatrixIdentity(mIdentity);
                    pSingleRootNode->mTransformList.push_back(mIdentity);
                    pSingleRootNode->BodyType = OG_SUBMESH_DUMMY;
                    pSingleRootNode->nIdx = n;
                    pSingleRootNode->nIdxParent = -1;
                    AttachChild(pSingleRootNode, pCurNode);
                    AttachChild(pSingleRootNode, pRoot);
                    pRoot = pSingleRootNode;
                    m_Nodes.push_back(pSingleRootNode);
                    bRootCreated = true;
                }
            }
            else
            {
                pRoot = pCurNode;
            }
        }
        else
        {
            OGModelNode* pNodeParent = m_Nodes[curParent];
            pNodeParent->nIdx = curParent;
            AttachChild(pNodeParent, pCurNode);
        }
    }
    return pRoot;
}


void COGModelSkeleton::GetWorldMatrix(OGMatrix& _mOut, unsigned int _NodeId, unsigned int _Frame) const
{
    OGModelNode* pNode = m_Nodes[_NodeId];
    if (pNode->mTransformList.size() > _Frame)
        _mOut = pNode->mTransformList[_Frame];
    else
        _mOut = pNode->mTransformList[0];

    if(pNode->nIdxParent < 0)
        return;

    OGMatrix mTmp;
    GetWorldMatrix(mTmp, pNode->nIdxParent, _Frame);
    MatrixMultiply(_mOut, _mOut, mTmp);
}


unsigned int COGModelSkeleton::GetNumNodes () const
{
    return (unsigned int)m_Nodes.size();
}


const OGModelNode* COGModelSkeleton::GetNode (unsigned int _NodeId) const
{
    if (_NodeId < m_Nodes.size())
    {
        return m_Nodes[_NodeId];
    }
    return NULL;
}


void COGModelSkeleton::AttachChild(OGModelNode* _pParent, OGModelNode* _pChild)
{
    _pChild->pParent = _pParent;
    _pChild->nIdxParent = _pParent->nIdx;
    _pParent->ChildNodes.push_back(_pChild);
}
