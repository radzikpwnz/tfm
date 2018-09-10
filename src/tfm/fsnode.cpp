#include "common.h"

#include "actions.h"
#include "env.h"

#include "fsnode.h"


// Special nodes display names
std::wstring FSNode::SpecialDisplayNames[SPECIAL_TYPE_LAST] = {
    L"",
    L"Desktop",
    L"My Documents",
    L"Computer"
};

// Special nodes paths
std::wstring FSNode::SpecialPaths[SPECIAL_TYPE_LAST];


// Common initialization
void
FSNode::initCommon()
{
    SpecialPaths[NONE] = std::wstring();
    SpecialPaths[DESKTOP] = GetDesktopPath();
    SpecialPaths[MY_DOCUMENTS] = GetMyDocumentsPath();
    SpecialPaths[MY_COMPUTER] = std::wstring();
}

// Get node display name
std::wstring const&
FSNode::getDisplayName() const
{
    switch ( mType )
    {
        case FILE:
        case DIR:
            return mPath;
        case LOGICAL_DRIVE:
            return mPath;
        case SPECIAL:
            return SpecialDisplayNames[getSpecialType()];
        default:
            return SpecialDisplayNames[0];
    }
}

// Rebuild childs list for "My Computer" node
void
FSNode::rebuildChildsListMyComputer(std::vector<FSNode>& resList)
{
    int bufSize = GetLogicalDriveStrings(0, NULL);
    std::vector<wchar_t> drvlist(bufSize);
    GetLogicalDriveStrings(256, drvlist.data());

    int i = 0;
    while ( drvlist[i] != L'\0' )
    {
        wchar_t* cur = &drvlist[i];
        i += wcslen(cur) + 1;

        fs::path path = fs::path(cur);
        if ( !IsPathExists(path) )
        {
            continue;
        }

        FSNode node(FSNode::Type::LOGICAL_DRIVE, path.root_name().native());
        resList.push_back(std::move(node));
    }
}

// Rebuild childs list for directory node
void
FSNode::rebuildChildsListDir(std::vector<FSNode>& resList, bool dirsOnly)
{
    fs::path fullPath = GetFSNodeFullPath(*this);

    if ( !IsPathExists(fullPath) )
    {
        return;
    }

    std::error_code err;
    fs::directory_iterator it(fullPath, err);
    if ( err )
    {
        return;
    }

    for ( fs::directory_entry entry : it )
    {
        if ( entry.is_directory() )
        {
            FSNode node(DIR, entry.path().filename().wstring());
            resList.push_back(std::move(node));
        }
    }

    if ( dirsOnly )
    {
        return;
    }

    for ( fs::directory_entry entry : fs::directory_iterator(fullPath) )
    {
        Type type;
        if ( entry.is_directory() )
        {
            continue;
        } else if ( entry.is_regular_file() )
        {
            type = FILE;
        } else
        {
            continue;
        }

        FSNode node(type, entry.path().filename().wstring());
        resList.push_back(std::move(node));
    }
}

// Rebuild node childs list
void
FSNode::rebuildChildsList(std::vector<FSNode>& resList, bool dirsOnly)
{
    resList.clear();

    if ( mType == SPECIAL )
    {
        switch ( mSpecialType )
        {
            case DESKTOP:
            case MY_DOCUMENTS:
                rebuildChildsListDir(resList, dirsOnly);
                break;
            case MY_COMPUTER:
                rebuildChildsListMyComputer(resList);
                break;
            default:
                assert(false);
                break;
        }
    } else
    {
        rebuildChildsListDir(resList, dirsOnly);
    }

    for ( unsigned j = 0; j < resList.size(); j++ )
    {
        resList[j].setParent(this);
    }
}

// Rebuild node childs list in internal list
void
FSNode::rebuildChildsList(bool dirsOnly)
{
    rebuildChildsList(mChilds, dirsOnly);
}

// Clear node internal childs list
void
FSNode::clearChildsList()
{
    mChilds.clear();
}

// Check if node (and it's childs if rec == true) has child folders
void
FSNode::checkIfHasChilds(bool rec)
{
    mHasChildFolders = false;

    if ( mSpecialType == MY_COMPUTER )
    {
        mHasChildFolders = true;
        if ( rec && getChildsList().size() != 0 )
        {
            CheckIfNodesHasChilds(getChildsList(), true);
        }
        return;
    }

    fs::path fullPath = GetFSNodeFullPath(*this);

    if ( !IsPathExists(fullPath) )
    {
        return;
    }

    std::error_code err;
    fs::directory_iterator it(fullPath, err);
    if ( err )
    {
        return;
    }

    for ( fs::directory_entry entry : it )
    {
        if ( entry.is_directory() )
        {
            mHasChildFolders = true;
            if ( rec && getChildsList().size() != 0 )
            {
                CheckIfNodesHasChilds(getChildsList(), true);
            }
            
            return;
        }
    }
}


// Is path exists
bool
IsPathExists(fs::path path)
{
    std::error_code err;
    bool res = fs::exists(path, err);
    return !err && res;
}

// Is path exists and readable
bool
IsPathReadable(fs::path path)
{
    std::error_code err;
    fs::file_status status = fs::status(path, err);
    if ( err )
    {
        return false;
    }
    fs::perms perms = status.permissions();
    return (perms & fs::perms::owner_read) != fs::perms::none;
}

// Check if nodes has child folders
void
CheckIfNodesHasChilds(std::vector<FSNode>& nodes, bool rec)
{
    for ( unsigned i = 0; i < nodes.size(); i++ )
    {
        nodes[i].checkIfHasChilds(rec);
    }
}

// Get node full path
fs::path
GetFSNodeFullPath(FSNode const& node)
{
    fs::path res;
    std::list<FSNode const*> list;

    for ( FSNode const* curNode = &node; curNode != nullptr; curNode = curNode->getParent()  )
    {
        list.push_front(curNode);
    }

    for ( auto it = list.begin(); it != list.end(); it++ )
    {
        FSNode const* curNode = *it;

        std::wstring const& relPath = curNode->getRelPath();
        res += relPath;

        if ( curNode->getType() != FSNode::FILE
             && !relPath.empty()
             && *it != list.back() )
        {
            res += L"\\";
        }
    }

    if ( !res.has_root_directory() )
    {
        res += L"\\";
    }

    return res;
}

// Make nodes list from path
void
MakeFSNodesListFromPath(std::wstring pathStr, std::vector<FSNode>& resList)
{
    resList.clear();

    fs::path path(pathStr);

    if ( !IsPathExists(path) )
    {
        return;
    }

    resList.push_back(FSNode(FSNode::MY_COMPUTER));

    int i = 0;
    for ( fs::path::iterator iter = path.begin(); iter != path.end(); iter++, i++ )
    {
        if ( i == 0 )
        {
            resList.push_back(FSNode(FSNode::LOGICAL_DRIVE, (*iter).wstring()));
        } else if ( i == 1 )
        {
            continue;
        } else
        {
            resList.push_back(FSNode(FSNode::DIR, *iter));
        }
    }

    for ( unsigned j = 1; j < resList.size(); j++ )
    {
        resList[j].setParent(&resList[j - 1]);
    }
}



