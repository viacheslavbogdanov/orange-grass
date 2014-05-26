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
#include "ogsettingsreader.h"
#include "ogresourcefile.h"


COGSettingsReader::COGSettingsReader()
{
}


COGSettingsReader::~COGSettingsReader()
{
}


// open settings source.
IOGSettingsSource* COGSettingsReader::OpenSource (const std::string& _File)
{
    COGResourceFile ResFile;
    if (!ResFile.OpenForRead(_File))
    {
        OG_LOG_ERROR("COGSettingsReader cannot open file %s", _File.c_str());
        return NULL;
    }

    COGSettingsSource* pSource = new COGSettingsSource;
    pSource->pSource = new TiXmlDocument (_File.c_str());
    if (!pSource->pSource->LoadFile (ResFile.DataPtr(), ResFile.Size(), TIXML_DEFAULT_ENCODING))
    {
        OG_LOG_ERROR("COGSettingsReader cannot load or parse file %s", _File.c_str());
        OG_SAFE_DELETE(pSource->pSource);
        OG_SAFE_DELETE(pSource);
        ResFile.Close();
        return NULL;
    }
    ResFile.Close();

    pSource->pDoc = new TiXmlHandle (pSource->pSource);
    return pSource;
}


// close source.
void COGSettingsReader::CloseSource (IOGSettingsSource* _pSource)
{
    COGSettingsSource* pSource = (COGSettingsSource*)_pSource;

    OG_SAFE_DELETE(pSource->pDoc);
    OG_SAFE_DELETE(pSource->pSource);
    OG_SAFE_DELETE(pSource);
}


// read group node.
IOGGroupNode* COGSettingsReader::OpenGroupNode (
    IOGSettingsSource* _pSource, 
    IOGGroupNode* _pParent, 
    const std::string& _Alias)
{
    COGGroupNode* pGroup = new COGGroupNode;
    pGroup->index = 0;
    pGroup->name = _Alias;
    pGroup->pSource = _pSource;
    if (_pParent)
    {
        pGroup->pParent = _pParent;
        pGroup->pNode = new TiXmlHandle(((COGGroupNode*)pGroup->pParent)->pNode->Child ( pGroup->name.c_str(), pGroup->index ));
        pGroup->pElement = pGroup->pNode->Element();
        if (pGroup->pElement == NULL)
        {
            OG_SAFE_DELETE(pGroup->pNode);
            OG_SAFE_DELETE(pGroup);
            return NULL;
        }
    }
    else
    {
        pGroup->pParent = NULL;
        pGroup->pNode = new TiXmlHandle(((COGSettingsSource*)_pSource)->pDoc->Child ( pGroup->name.c_str(), pGroup->index ));
        pGroup->pElement = pGroup->pNode->Element();
        if (pGroup->pElement == NULL)
        {
            OG_SAFE_DELETE(pGroup->pNode);
            OG_SAFE_DELETE(pGroup);
            return NULL;
        }
    }
    return pGroup;
}


// close group node.
void COGSettingsReader::CloseGroupNode (IOGGroupNode* _pNode)
{
    if (_pNode)
    {
        COGGroupNode* pGroup = (COGGroupNode*)_pNode;
        OG_SAFE_DELETE(pGroup->pNode);
        OG_SAFE_DELETE(pGroup);
    }
}


// read next group node.
IOGGroupNode* COGSettingsReader::ReadNextNode (IOGGroupNode* _pNode)
{
    TiXmlHandle* pNewNode = new TiXmlHandle(((COGGroupNode*)_pNode->pParent)->pNode->Child ( _pNode->name.c_str(), _pNode->index + 1 ));
    if (pNewNode->Node())
    {
        COGGroupNode* pNode = (COGGroupNode*)_pNode;
        OG_SAFE_DELETE(pNode->pNode);
        pNode->pNode = pNewNode;
        pNode->index++;
        pNode->pElement = pNode->pNode->Element();
        return pNode;
    }
    else
    {
        OG_SAFE_DELETE(pNewNode);
        CloseGroupNode(_pNode);
        return NULL;
    }
}


// read string parameter.
std::string COGSettingsReader::ReadStringParam (IOGGroupNode* _pGroup, const std::string& _Alias)
{
    COGGroupNode* pNode = (COGGroupNode*)_pGroup;
    if (!pNode->pElement)
        return std::string("");

    return std::string(pNode->pElement->Attribute (_Alias.c_str()));
}


// read int parameter.
int COGSettingsReader::ReadIntParam (IOGGroupNode* _pGroup, const std::string& _Alias)
{
    int val = 0;
    COGGroupNode* pNode = (COGGroupNode*)_pGroup;
    if (!pNode->pElement)
        return val;

    pNode->pElement->Attribute (_Alias.c_str(), &val);
    return val;
}


// read float parameter.
float COGSettingsReader::ReadFloatParam (IOGGroupNode* _pGroup, const std::string& _Alias)
{
    double val = 0.0;
    COGGroupNode* pNode = (COGGroupNode*)_pGroup;
    if (!pNode->pElement)
        return 0.0f;

    pNode->pElement->Attribute (_Alias.c_str(), &val);
    return (float)val;
}


// read OGVec2 parameter.
OGVec2 COGSettingsReader::ReadVec2Param (
    IOGGroupNode* _pGroup, 
    const std::string& _AliasX, 
    const std::string& _AliasY)
{
    OGVec2 vOut;
    vOut.x = ReadFloatParam(_pGroup, _AliasX);
    vOut.y = ReadFloatParam(_pGroup, _AliasY);
    return vOut;
}


// read OGVec3 parameter.
OGVec3 COGSettingsReader::ReadVec3Param (
    IOGGroupNode* _pGroup, 
    const std::string& _AliasX, 
    const std::string& _AliasY,
    const std::string& _AliasZ)
{
    OGVec3 vOut;
    vOut.x = ReadFloatParam(_pGroup, _AliasX);
    vOut.y = ReadFloatParam(_pGroup, _AliasY);
    vOut.z = ReadFloatParam(_pGroup, _AliasZ);
    return vOut;
}


// read OGVec4 parameter.
OGVec4 COGSettingsReader::ReadVec4Param (
    IOGGroupNode* _pGroup, 
    const std::string& _AliasX, 
    const std::string& _AliasY,
    const std::string& _AliasZ,
    const std::string& _AliasW)
{
    OGVec4 vOut;
    vOut.x = ReadFloatParam(_pGroup, _AliasX);
    vOut.y = ReadFloatParam(_pGroup, _AliasY);
    vOut.z = ReadFloatParam(_pGroup, _AliasZ);
    vOut.w = ReadFloatParam(_pGroup, _AliasW);
    return vOut;
}


// save settings source.
bool COGSettingsReader::SaveSource (IOGSettingsSource* _pSource, const std::string& _File)
{
    COGSettingsSource* pSource = (COGSettingsSource*)_pSource;
    return pSource->pSource->SaveFile(_File.c_str());
}


// write string parameter.
void COGSettingsReader::WriteStringParam (IOGGroupNode* _pGroup, const std::string& _Alias, const std::string& _Value)
{
    COGGroupNode* pNode = (COGGroupNode*)_pGroup;
    if (!pNode->pElement)
        return;

    pNode->pElement->SetAttribute (_Alias.c_str(), _Value.c_str());
}


// write int parameter.
void COGSettingsReader::WriteIntParam (IOGGroupNode* _pGroup, const std::string& _Alias, int _Value)
{
    COGGroupNode* pNode = (COGGroupNode*)_pGroup;
    if (!pNode->pElement)
        return;

    pNode->pElement->SetAttribute (_Alias.c_str(), _Value);
}


// write float parameter.
void COGSettingsReader::WriteFloatParam (IOGGroupNode* _pGroup, const std::string& _Alias, float _Value)
{
    COGGroupNode* pNode = (COGGroupNode*)_pGroup;
    if (!pNode->pElement)
        return;

    pNode->pElement->SetDoubleAttribute (_Alias.c_str(), (double)_Value);
}


// write OGVec2 parameter.
void COGSettingsReader::WriteVec2Param (
    IOGGroupNode* _pGroup, 
    const std::string& _AliasX, 
    const std::string& _AliasY,
    const OGVec2& _Value)
{
    WriteFloatParam(_pGroup, _AliasX, _Value.x);
    WriteFloatParam(_pGroup, _AliasY, _Value.y);
}


// write OGVec3 parameter.
void COGSettingsReader::WriteVec3Param (
    IOGGroupNode* _pGroup, 
    const std::string& _AliasX, 
    const std::string& _AliasY,
    const std::string& _AliasZ,
    const OGVec3& _Value)
{
    WriteFloatParam(_pGroup, _AliasX, _Value.x);
    WriteFloatParam(_pGroup, _AliasY, _Value.y);
    WriteFloatParam(_pGroup, _AliasZ, _Value.z);
}


// write OGVec4 parameter.
void COGSettingsReader::WriteVec4Param (
    IOGGroupNode* _pGroup, 
    const std::string& _AliasX, 
    const std::string& _AliasY,
    const std::string& _AliasZ,
    const std::string& _AliasW,
    const OGVec4& _Value)
{
    WriteFloatParam(_pGroup, _AliasX, _Value.x);
    WriteFloatParam(_pGroup, _AliasY, _Value.y);
    WriteFloatParam(_pGroup, _AliasZ, _Value.z);
    WriteFloatParam(_pGroup, _AliasW, _Value.w);
}
