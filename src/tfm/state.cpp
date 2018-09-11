// Copyright (c) Eltechs.
// All rights reserved.
//
// This code is licensed under the MIT License.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial
// portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "common.h"

#include "core.h"
#include "fsnode.h"

#include "state.h"


// Root FS nodes
static thread_local std::vector<FSNode> gRootFSNodes;

// FS nodes tree root
static thread_local std::vector<FSNode> gFSNodesTreeRoot;

// Current path
static thread_local fs::path gCurPath;
// Current path FS nodes
static thread_local std::vector<FSNode> gCurPathFSNodes;

// Current content FS nodes
static thread_local std::vector<FSNode> gCurContentFSNodes;


// Init root FS nodes
bool
InitRootFSNodes()
{
    gRootFSNodes.clear();

    gRootFSNodes.push_back(FSNode(FSNode::DESKTOP));
    gRootFSNodes.push_back(FSNode(FSNode::MY_DOCUMENTS));
    gRootFSNodes.push_back(FSNode(FSNode::MY_COMPUTER));

    return true;
}

// Init FS nodes tree
bool
InitFSNodesTree()
{
    gFSNodesTreeRoot.clear();

    gFSNodesTreeRoot.push_back(FSNode(FSNode::DESKTOP));
    gFSNodesTreeRoot.push_back(FSNode(FSNode::MY_DOCUMENTS));
    gFSNodesTreeRoot.push_back(FSNode(FSNode::MY_COMPUTER));
    FSNode& myCompNode = gFSNodesTreeRoot.back();

    myCompNode.rebuildChildsList();

    CheckIfNodesHasChilds(gFSNodesTreeRoot, true);

    return true;
}

// Get FS nodes tree
std::vector<FSNode> const&
GetFSNodesTree()
{
    return gFSNodesTreeRoot;
}

// Get root FS nodes
std::vector<FSNode> const&
GetRootFSNodes()
{
    return gRootFSNodes;
}

// Get current path FS nodes
std::vector<FSNode> const&
GetCurPathFSNodes()
{
    return gCurPathFSNodes;
}

// Get current content FS nodes
std::vector<FSNode> const&
GetCurContentFSNodes()
{
    return gCurContentFSNodes;
}

// Get current path
fs::path const&
GetCurPath()
{
    return gCurPath;
}

// Set current path
static void
SetCurPath(fs::path path)
{
    if ( !path.empty() && !path.has_root_directory() )
    {
        path.append(L"\\");
    }
    gCurPath = (path / L"1.txt").parent_path();
}


// On navigate handler (current path change)
static void
OnNavigate()
{
    gCurPathFSNodes.back().rebuildChildsList(gCurContentFSNodes);
    InstanceManager::getCurrent()->getMainWnd()->onCurPathChange();
    InstanceManager::getCurrent()->getMainWnd()->onContentUpdate();
}

// On navigate up handler
void
NavigateUp(FSNode const* node)
{
    if ( node == nullptr )
    {
        node = ( gCurPathFSNodes.size() == 1 ) ? &gCurPathFSNodes.back() : &gCurPathFSNodes[gCurPathFSNodes.size() - 2];
    }

    while ( &gCurPathFSNodes.back() != node )
    {
        gCurPathFSNodes.pop_back();
    }

    SetCurPath(GetFSNodeFullPath(gCurPathFSNodes.back()));
    OnNavigate();
}

// On navigate down handler
void
NavigateDown(std::wstring dir)
{
    fs::path newPath = gCurPath / dir;

    if ( !IsPathExists(newPath) )
    {
        return;
    }

    gCurPathFSNodes.push_back(FSNode(FSNode::DIR, dir));
    for ( unsigned j = 1; j < gCurPathFSNodes.size(); j++ )
    {
        gCurPathFSNodes[j].setParent(&gCurPathFSNodes[j - 1]);
    }

    SetCurPath(GetFSNodeFullPath(gCurPathFSNodes.back()));
    OnNavigate();
}

// Refresh current path's content
void NavigateRefresh()
{
    gCurPathFSNodes.back().rebuildChildsList(gCurContentFSNodes);
    InstanceManager::getCurrent()->getMainWnd()->onContentUpdate();
}

// Navigate to special node
void
NavigateToSpecialNode(FSNode::SpecialType specialType)
{
    gCurPathFSNodes.clear();
    gCurPathFSNodes.push_back(FSNode(specialType));
    SetCurPath(GetFSNodeFullPath(gCurPathFSNodes.back()));
    OnNavigate();
}

// Navigate to root node
void
NavigateToRootNode(FSNode* node)
{
    gCurPathFSNodes.clear();
    gCurPathFSNodes.push_back(*node);
    SetCurPath(GetFSNodeFullPath(gCurPathFSNodes.back()));
    OnNavigate();
}

// Navigate by selecting element from tree
void
NavigateFromTree(FSNode* node)
{
    gCurPathFSNodes.clear();

    std::list<FSNode const*> list;
    for ( FSNode const* curNode = node; curNode != nullptr; curNode = curNode->getParent() )
    {
        list.push_front(curNode);
    }

    for ( auto it = list.begin(); it != list.end(); it++ )
    {
        FSNode const* curNode = *it;
        gCurPathFSNodes.push_back(*curNode);
    }

    SetCurPath(GetFSNodeFullPath(gCurPathFSNodes.back()));
    OnNavigate();
}

// Navigate to path
static void
NavigateToPath(fs::path path)
{
    if ( !IsPathExists(path) )
    {
        return;
    }

    SetCurPath(path);
    MakeFSNodesListFromPath(gCurPath, gCurPathFSNodes);
    OnNavigate();
}

// Tree expanding handler
void
TreeExpanding(HTREEITEM treeItem, FSNode* node)
{
    node->rebuildChildsList(true);
    CheckIfNodesHasChilds(node->getChildsList(), false);
    InstanceManager::getCurrent()->getMainWnd()->onTreeExpanding(treeItem, *node);
}

// Tree collapsed handler
void
TreeCollapsed(HTREEITEM treeItem, FSNode* node)
{
    UNREFERENCED_PARAMETER(treeItem);

    node->clearChildsList();
}

// Tree full refresh
void
TreeFullRefresh()
{
    CheckIfNodesHasChilds(gFSNodesTreeRoot, true);
    InstanceManager::getCurrent()->getMainWnd()->onTreeRefresh();
}

// Set current path from cmdline argument
void
SetCurPathFromCmdlineArg(std::wstring const& pathStr)
{
    fs::path path(pathStr);

    Debug::logVerbose(L"Cmdline path: %s\n", pathStr.c_str());

    if ( !IsPathExists(path) )
    {
        SetCurPath(L"");
        gCurPathFSNodes.clear();
        gCurPathFSNodes.push_back(FSNode(FSNode::MY_COMPUTER));
        OnNavigate();
        return;
    }

    fs::file_status status = fs::status(path);
    if ( status.type() == fs::file_type::regular )
    {
        // TODO: shell execute
    } else if ( status.type() == fs::file_type::directory )
    {
        NavigateToPath(path);
    }
}