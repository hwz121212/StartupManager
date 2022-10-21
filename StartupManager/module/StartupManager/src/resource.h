/*
 * this file is part of Startup Manager
 * Copyright (C) 2004-2008 Glenn Van Loon, glenn@startupmanager.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef RESOURCE_H
#define RESOURCE_H

#include "..\Win32++\Default_Resource.h"

// menu IDs
#define IDM_FILE_ADD					201
#define IDM_FILE_BTNEW					202
#define IDM_FILE_BTOPEN					203
#define IDM_FILE_LOG					204
#define IDM_FILE_XPTHHTML				205
#define IDM_FILE_XPTVHTML				206
#define IDM_FILE_XPTXML					207
#define IDM_FILE_MSCONFIG				208
#define IDM_FILE_REGEDIT				209
#define IDM_FILE_EXIT					210
#define IDM_EDIT_DELETE					211
#define IDM_EDIT_RENAME					212
#define IDM_EDIT_RUN					213
#define IDM_EDIT_PROPERTIES				214
#define IDM_EDIT_FILEPROPERTIES			215
#define IDM_VIEW_ICON					216
#define IDM_VIEW_SMALL					217
#define IDM_VIEW_LIST					218
#define IDM_VIEW_REPORT					219
#define IDM_VIEW_REFRESH				220
#define IDM_SYSTEM_SETTINGS				221
#define IDM_SYSTEM_SHUTDOWN				222
#define IDM_SYSTEM_POWEROFF				223
#define IDM_SYSTEM_REBOOT				224
#define IDM_SYSTEM_LOGOFF				225
#define IDM_HELP_CONTENTS				226
#define IDM_HELP_WEBSITE				227
#define IDM_HELP_SUPPORT				228
#define IDM_HELP_UPDATE					229
#define IDM_HELP_ABOUT					230

#define IDM_EDIT_TAB					250
#define IDM_EDIT_APP					251

// dialog IDs
#define IDD_ADD							301
#define IDD_LOG							302
#define IDD_ABOUT						303
#define IDD_RENAME						304
#define IDD_PROPERTIES					305
#define IDD_DRAGDROP					306
#define IDD_ORDER						307
#define IDD_NEWBAT						308
#define IDD_ADDTOBATCH					309
#define IDD_UPDATE						310
#define IDD_GENERALSETTINGS				311
#define IDD_ADVANCEDSETTINGS			312

// image IDs
#define IDI_HEADER						401
#define IDI_TOOLBAR						402
#define IDI_TOOLBAR_HOT					403
#define IDI_TOOLBAR_DIS					404
#define IDI_TREE						405
#define IDI_INVALID						406
#define IDI_ABOUT						407
#define IDI_DELETE						408
#define IDI_HELP						409
#define IDI_HYBERNATE					410
#define IDI_LOGOFF						411
#define IDI_NEW							412
#define IDI_PROPERTIES					413
#define IDI_REFRESH						414
#define IDI_RENAME						415
#define IDI_RESTART						416
#define IDI_RUN							417
#define IDI_SHUTDOWN					418
#define IDI_UPDATE						419
#define IDI_DONATE						420
#define IDI_UPARROW						421
#define IDI_DOWNARROW					422

// control IDs
#define IDC_SECTION						501
#define IDC_NEWNAME						502
#define IDC_FILENAME					503
#define IDC_COMPANY						504
#define IDC_COPYRIGHT					505
#define IDC_FVERSION					506
#define IDC_INAME						507
#define IDC_ONAME						508
#define IDC_PNAME						509
#define IDC_PVERSION					510
#define IDC_DVERSION					511
#define IDC_FILEOS						512
#define IDC_FTYPE						513
#define IDC_DESCRIPTION					514
#define IDC_SHOWICON					515
#define IDC_TEXT						516
#define IDC_EXP1						517
#define IDC_EXP2						518
#define IDC_PATH						519
#define IDC_NAME						520
#define IDC_FLAGS						521
#define IDC_LINE						522
#define IDC_NUMBER						523
#define IDC_FILETAB						524
#define IDC_BATCONTENT					525
#define IDC_CHWAIT						526
#define IDC_GOOGLE						527
#define IDC_HOMEPAGE					528
#define IDC_LICENSE						529
#define IDC_DMSCONFIG					530
#define IDC_DPRIVATE					531
#define IDC_WIN32XX						532
#define IDC_CURRENT						533
#define IDC_LATEST						534
#define IDC_INFORMATION					535
#define IDC_DONATE						536
#define IDC_STATICNAME					537
#define IDC_SIZEPOS						538
#define IDC_SORTINFO					539
#define IDC_AUPDATE						540
#define IDC_USEXSL						541

// button IDs
#define IDADD							601
#define IDCLEAR							602
#define IDEXP							603
#define IDUP							604
#define IDDOWN							605
#define IDDELETE						606
#define IDNEW							607
#define IDOPEN							608
#define IDAPPLY							609

// toolbar button text
#define STR_FILE_ADD					701
#define STR_EDIT_DELETE					702
#define STR_EDIT_RENAME					703
#define STR_EDIT_RUN					704
#define STR_EDIT_PROPERTIES				705
#define STR_VIEW_REFRESH				706
#define STR_HELP_CONTENTS				707
#define STR_HELP_ABOUT					708

// string IDs on a per dialog basis
#define MSG_INFORMATION					801
#define MSG_WARNING						802
#define MSG_ERROR						803
#define MSG_QUESTION					804
#define DLGADD_TT_IDOK					805
#define DLGADD_TT_IDBROWSE				806
#define DLGADD_TT_IDC_SECTION			807
#define DLGADD_TT_IDC_NAME				808
#define DLGADD_TT_IDC_PATH				809
#define DLGADD_TT_IDC_FLAGS				810
#define DLGADD_MSG_SPACES				811
#define DLGADD_MSG_EMPTYFIELD			812
#define DLGADD_MSG_FAILED				813
#define DLGADD_MSG_EMPTYFIELDBHO		814
#define DLGADD_MSG_FAILEDBHO			815
#define DLGBTORDERC_TT_IDC_NAME			816
#define DLGBTORDERC_TT_IDC_SECTION		817
#define DLGBTORDERA_TT_IDOK				818
#define DLGBTORDERA_TT_IDBROWSE			819
#define DLGBTORDERA_TT_IDC_PATH			820
#define DLGBTORDERA_TT_IDC_FLAGS		821
#define DLGBTORDER_TT_IDUP				822
#define DLGBTORDER_TT_IDDOWN			823
#define DLGBTORDER_TT_IDADD				824
#define DLGBTORDER_TT_IDDELETE			825
#define DLGBTORDER_TT_IDNEW				826
#define DLGBTORDER_TT_IDOPEN			827
#define DLGBTORDER_TT_IDOK				828
#define DLGBTORDER_TT_IDCANCEL			829
#define DLGBTORDER_TT_IDC_CHWAIT		830
#define DLGBTORDERA_MSG_EMPTYFIELD		831
#define DLGBTORDER_MSG_NOBATCH			832
#define DLGBTORDER_STR_LVCOLUMN			833
#define DLGBTORDER_MSG_ADDFAILED		834
#define DLGBTORDER_MSG_BATCH			835
#define DLGBTORDER_MSG_EMPTY			836
#define DLGBTORDER_MSG_QADD				837
#define DLGDROP_TT_IDOK					838
#define DLGDROP_TT_IDC_SECTION			839
#define DLGDROP_TT_IDEXP				840
#define DLGDROP_TT_IDC_NAME				841
#define DLGDROP_TT_IDC_FLAGS			842
#define DLGDROP_STR_INFO1				843
#define DLGDROP_STR_INFO2				844
#define DLGDROP_MSG_SPACES				845
#define DLGDROP_MSG_FAILED				846
#define DLGLOG_TT_IDCLEAR				847
#define DLGLOG_STR_COL1					848
#define DLGLOG_STR_COL2					849
#define DLGLOG_STR_COL3					850
#define DLGLOG_STR_COL4					851
#define DLGLOG_STR_COL5					852
#define DLGLOG_STR_COL6					853
#define DLGLOG_STR_COUNT				854
#define DLGLOG_STR_EMPTY				855
#define DLGPROP_STR_TITLE				856
#define DLGPROP_TT_IDOK					857
#define DLGPROP_TT_IDC_FILENAME			858
#define DLGPROP_TT_IDC_GOOGLE			859
#define DLGSETTINGS_TT_IDOK				860
#define DLGSETTINGS_TT_IDAPPLY			861
#define DLGSETTINGS_TT_IDCANCEL			862
#define DLGSETTINGS_MSG_PRIVATE			863
#define DLGUPDATE_TT_IDC_INFO			864
#define DLGUPDATE_STR_CHECKING			865
#define DLGUPDATE_STR_YES				866
#define DLGUPDATE_STR_NO				867
#define DLGUPDATE_MSG_FAILED			868
#define DLGRENAME_TT_IDOK				869
#define DLGRENAME_TT_IDC_NEWNAME		870
#define DLGRENAME_STR_TITLE				871
#define DLGRENAME_MSG_FAILED			872
#define DLGRENAME_MSG_EMPTYFIELD		873
#define STR_TITLE						874
#define STR_DELETE						875
#define MSG_DELETESYSFILE				876
#define MSG_DELETEFILE					877
#define MSG_DELETEFAILED				878
#define MSG_HELPMISSING					879
#define STR_COL1						880 
#define STR_COL2						881
#define STR_COL3						882
#define STR_COL4						883
#define MSG_DISABLESYSFILE				884
#define MSG_DISABLECORRUPTED			885
#define MSG_DISABLEFAILED				886
#define STR_CONTEXT1					887
#define STR_CONTEXT2					888
#define STR_CONTEXT3					889
#define STR_CONTEXT4					890
#define STR_CONTEXT5					891
#define STR_CONTEXT6					892
#define STR_CONTEXT7					893
#define STR_CONTEXT8					894
#define STR_DISABLED					895
#define STR_TREETITLE					896
#define STR_UPDATE1						897
#define STR_UPDATE2						898
#define STR_UPDATETITLE					899
#define SETTINGSDLG_STR_TITLE			900
#define SETTINGSDLG_STR_TITLEP1			901
#define SETTINGSDLG_STR_TITLEP2			902

#define IDW_STATIC                      -1

#endif // RESOURCE_H
