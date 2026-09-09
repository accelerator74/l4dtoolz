/**
 * vim: set ts=4 sw=4 tw=99 noet :
 * =============================================================================
 * MemoryUtils
 * Copyright (C) 2004-2011 AlliedModders LLC., 2011 ProdigySim
 * All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, the authors give you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 */

#ifndef _INCLUDE_SOURCEMOD_MEMORYUTILS_H_
#define _INCLUDE_SOURCEMOD_MEMORYUTILS_H_

#include <stdio.h>
#include <stdint.h>

#include "khook/memory.hpp"
#include "khook.hpp"

#if defined LINUX || defined OSX
#include <vector>
#include "sm_symtable.h"
#endif

#if defined OSX
#include <CoreServices/CoreServices.h>
#endif

#if defined LINUX || defined OSX
struct LibSymbolTable
{
	SymbolTable table;
	uintptr_t lib_base;
	uint32_t last_pos;
};
#endif

class MemoryUtils
{
public:
	MemoryUtils();
	~MemoryUtils();
	void *ResolveSymbol(void *handle, const char *symbol);

#if defined LINUX || defined OSX
private:
	std::vector<LibSymbolTable *> m_SymTables;
#if defined OSX
	struct dyld_all_image_infos *m_ImageList;
	SInt32 m_OSXMajor;
	SInt32 m_OSXMinor;
#endif
#endif
};

extern MemoryUtils g_MemUtils;

#endif // _INCLUDE_SOURCEMOD_MEMORYUTILS_H_
