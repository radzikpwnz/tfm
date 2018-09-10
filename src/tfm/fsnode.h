#pragma once

#include "common.h"

class FSNode
{
public:
    enum Type
    {
        FILE,
        DIR,
        LOGICAL_DRIVE,
        SPECIAL,
        TYPE_LAST
    };

    enum SpecialType
    {
        NONE,
        DESKTOP,
        MY_DOCUMENTS,
        MY_COMPUTER,
        SPECIAL_TYPE_LAST
    };

public:
    FSNode() {}

    FSNode(Type type, std::wstring path)
    {
        mType = type;
        mSpecialType = NONE;
        mPath = path;
        mParent = nullptr;
        mHasChildFolders = false;
    }

    FSNode(SpecialType specialType)
    {
        mType = SPECIAL;
        mSpecialType = specialType;
        mPath = SpecialPaths[specialType];
        mParent = nullptr;
        mHasChildFolders = false;
    }

    //~FSNode() {}

public:
    static void initCommon();

    Type getType() const  { return mType; }

    SpecialType getSpecialType() const { return mSpecialType; }

    FSNode* getParent() const { return mParent; }
    void setParent(FSNode* parent) { mParent = parent; }

    std::wstring const& getDisplayName() const;

    std::wstring const& getRelPath() const { return mPath; }

    std::vector<FSNode> const& getChildsList() const { return mChilds; }

    std::vector<FSNode>& getChildsList() { return mChilds; }

    bool hasChildFolders() const { return mHasChildFolders; }

    void rebuildChildsListMyComputer(std::vector<FSNode>& resList);
    void rebuildChildsListDir(std::vector<FSNode>& resList, bool dirsOnly = false);
    void rebuildChildsList(std::vector<FSNode>& resList, bool dirsOnly = false);
    void rebuildChildsList(bool dirsOnly = false);
    void clearChildsList();

    void checkIfHasChilds(bool rec);

private:
    Type mType:3;
    SpecialType mSpecialType:3;
    bool mHasChildFolders:1;
    std::wstring mPath;
    FSNode* mParent;
    std::vector<FSNode> mChilds;

    static std::wstring SpecialDisplayNames[];
    static std::wstring SpecialPaths[];
};


bool IsPathExists(fs::path path);
bool IsPathReadable(fs::path path);

void CheckIfNodesHasChilds(std::vector<FSNode>& nodes, bool rec);
void MakeFSNodesListFromPath(std::wstring path, std::vector<FSNode>& resList);
fs::path GetFSNodeFullPath(FSNode const& node);
