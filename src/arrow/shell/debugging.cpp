/* 
 *  This file is a part of Hasky Lib.
 *
 *  Copyright (c) Pawe³ Kowal 2017 - 2021
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "debugging.h"

#include <windows.h>
#include <crtdbg.h>
#include <stdlib.h>

namespace arrow
{

void arrow::debug_heap_manager()
{
    _CrtSetDbgFlag(0
        | _CRTDBG_ALLOC_MEM_DF
        // | _CRTDBG_CHECK_ALWAYS_DF
        // | _CRTDBG_CHECK_CRT_DF
        // | _CRTDBG_DELAY_FREE_MEM_DF
        // | _CRTDBG_LEAK_CHECK_DF
    );
}

void arrow::disable_crash_dialog()
{
    SetErrorMode(SetErrorMode(0)
        | SEM_FAILCRITICALERRORS
        // | SEM_NOALIGNMENTFAULTEXCEPT
        | SEM_NOGPFAULTERRORBOX
        | SEM_NOOPENFILEERRORBOX
    );

    _set_abort_behavior(0, 0
        | _WRITE_ABORT_MSG
        | _CALL_REPORTFAULT
    );
}

void arrow::report_crash_to_stderr()
{
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
}

};
