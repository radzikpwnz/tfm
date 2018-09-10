#include "common.h"

static thread_local std::vector<FSNode> gRootFSNodes;

static thread_local std::vector<FSNode> gFSNodesTreeRoot;

static thread_local fs::path gCurPath;
static thread_local std::vector<FSNode> gCurPathFSNodes;

static thread_local std::vector<FSNode> gCurContentFSNodes;


bool
InitRootFSNodes()
{
    gRootFSNodes.clear();

    gRootFSNodes.push_back(FSNode(FSNode::DESKTOP));
    gRootFSNodes.push_back(FSNode(FSNode::MY_DOCUMENTS));
    gRootFSNodes.push_back(FSNode(FSNode::MY_COMPUTER));

    return true;
}

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

std::vector<FSNode> const&
GetFSNodesTree()
{
    return gFSNodesTreeRoot;
}

std::vector<FSNode> const&
GetRootFSNodes()
{
    return gRootFSNodes;
}

std::vector<FSNode> const&
GetCurPathFSNodes()
{
    return gCurPathFSNodes;
}

std::vector<FSNode> const&
GetCurContentFSNodes()
{
    return gCurContentFSNodes;
}


fs::path const&
GetCurPath()
{
    return gCurPath;
}

static void
SetCurPath(fs::path path)
{
    if ( !path.empty() && !path.has_root_directory() )
    {
        path.append(L"\\");
    }
    gCurPath = (path / L"1.txt").parent_path();
}

static void
OnNavigate()
{
    gCurPathFSNodes.back().rebuildChildsList(gCurContentFSNodes);
    InstanceManager::getCurrent()->getMainWnd()->onCurPathChange();
    InstanceManager::getCurrent()->getMainWnd()->onContentUpdate();
}

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

void NavigateRefresh()
{
    gCurPathFSNodes.back().rebuildChildsList(gCurContentFSNodes);
    InstanceManager::getCurrent()->getMainWnd()->onContentUpdate();
}

void
NavigateToSpecialNode(FSNode::SpecialType specialType)
{
    gCurPathFSNodes.clear();
    gCurPathFSNodes.push_back(FSNode(specialType));
    SetCurPath(GetFSNodeFullPath(gCurPathFSNodes.back()));
    OnNavigate();
}

void
NavigateToRootNode(FSNode* node)
{
    gCurPathFSNodes.clear();
    gCurPathFSNodes.push_back(*node);
    SetCurPath(GetFSNodeFullPath(gCurPathFSNodes.back()));
    OnNavigate();
}

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


void
TreeExpanding(HTREEITEM treeItem, FSNode* node)
{
    node->rebuildChildsList(true);
    CheckIfNodesHasChilds(node->getChildsList(), false);
    InstanceManager::getCurrent()->getMainWnd()->onTreeExpanding(treeItem, *node);
}

void
TreeCollapsed(HTREEITEM treeItem, FSNode* node)
{
    UNREFERENCED_PARAMETER(treeItem);

    node->clearChildsList();
}

void
TreeFullRefresh()
{
    CheckIfNodesHasChilds(gFSNodesTreeRoot, true);
    InstanceManager::getCurrent()->getMainWnd()->onTreeRefresh();
}

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