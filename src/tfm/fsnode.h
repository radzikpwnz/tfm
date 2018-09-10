#pragma once

#include "common.h"

// Filesystem node (FSNode)
class FSNode
{
public:
    // Node type
    enum Type
    {
        FILE,           // regular file
        DIR,            // directory
        LOGICAL_DRIVE,  // logical drive
        SPECIAL,        // special
        TYPE_LAST
    };

    // Special node types (for SPECIAL type)
    enum SpecialType
    {
        NONE,              // none (non-special)
        DESKTOP,           // Desktop
        MY_DOCUMENTS,      // My Documents
        MY_COMPUTER,       // My Computer
        SPECIAL_TYPE_LAST
    };

public:
    FSNode() {}

    // Non-special node constructor
    FSNode(Type type, std::wstring path)
    {
        mType = type;
        mSpecialType = NONE;
        mPath = path;
        mParent = nullptr;
        mHasChildFolders = false;
    }

    // Special node constructor
    FSNode(SpecialType specialType)
    {
        mType = SPECIAL;
        mSpecialType = specialType;
        mPath = SpecialPaths[specialType];
        mParent = nullptr;
        mHasChildFolders = false;
    }

public:
    static void initCommon();

public:
    // Get type
    Type getType() const  { return mType; }

    // Get special type
    SpecialType getSpecialType() const { return mSpecialType; }

    // Get parent node
    FSNode* getParent() const { return mParent; }
    // Set parent node
    void setParent(FSNode* parent) { mParent = parent; }

    // Get relative (from parent node) path
    std::wstring const& getRelPath() const { return mPath; }

    // Get childs list
    std::vector<FSNode> const& getChildsList() const { return mChilds; }
    std::vector<FSNode>& getChildsList() { return mChilds; }

    // Is node has child folders
    bool hasChildFolders() const { return mHasChildFolders; }


    std::wstring const& getDisplayName() const;

    void rebuildChildsListMyComputer(std::vector<FSNode>& resList);
    void rebuildChildsListDir(std::vector<FSNode>& resList, bool dirsOnly = false);
    void rebuildChildsList(std::vector<FSNode>& resList, bool dirsOnly = false);
    void rebuildChildsList(bool dirsOnly = false);
    void clearChildsList();

    void checkIfHasChilds(bool rec);

private:
    Type mType:3;                  // type
    SpecialType mSpecialType:3;    // special type
    bool mHasChildFolders:1;       // is node has child folders
    std::wstring mPath;            // relative path
    FSNode* mParent;               // parent node
    std::vector<FSNode> mChilds;   // child nodes

    static std::wstring SpecialDisplayNames[];
    static std::wstring SpecialPaths[];
};


bool IsPathExists(fs::path path);
bool IsPathReadable(fs::path path);

void CheckIfNodesHasChilds(std::vector<FSNode>& nodes, bool rec);
void MakeFSNodesListFromPath(std::wstring path, std::vector<FSNode>& resList);
fs::path GetFSNodeFullPath(FSNode const& node);
