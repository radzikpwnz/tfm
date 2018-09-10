#pragma once

#include "common.h"

bool InitRootFSNodes();
bool InitFSNodesTree();

std::vector<FSNode> const& GetFSNodesTree();
std::vector<FSNode> const& GetRootFSNodes();
fs::path const& GetCurPath();
std::vector<FSNode> const& GetCurPathFSNodes();
std::vector<FSNode> const& GetCurContentFSNodes();

void NavigateUp(FSNode const* node = nullptr);
void NavigateDown(std::wstring dir);
void NavigateRefresh();
void NavigateToSpecialNode(FSNode::SpecialType specialType);
void NavigateToRootNode(FSNode* node);
void NavigateFromTree(FSNode* node);

void TreeExpanding(HTREEITEM treeItem, FSNode* node);
void TreeCollapsed(HTREEITEM treeItem, FSNode* node);
void TreeFullRefresh();

void SetCurPathFromCmdlineArg(std::wstring const& pathStr);