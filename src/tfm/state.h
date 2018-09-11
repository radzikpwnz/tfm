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

#pragma once

#include "common.h"

#include "fsnode.h"

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