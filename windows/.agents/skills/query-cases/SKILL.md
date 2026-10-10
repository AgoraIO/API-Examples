---
name: query-cases
description: >
  Query existing Windows APIExample cases by feature name, Agora RTC API, category,
  scene label, or dialog class. Use before adding a case or when checking whether a
  feature is already implemented and fully registered. This skill is read-only and
  verifies the live MFC scene list, dialog routing, project membership, and localized
  labels instead of relying only on folder names or the architecture index.
---

# Query Cases - Windows

## Scope

Use this skill only to inspect the main Windows `APIExample` solution. It answers
which cases exist, which SDK APIs they demonstrate, where their dialog sources live,
and whether they are reachable from the MFC scene list. It must not edit or register
cases.

## Sources Of Truth

1. `ARCHITECTURE.md` for the fast case index
2. `APIExample/APIExample/APIExampleDlg.h` and `APIExample/APIExample/APIExampleDlg.cpp` for scene ownership and active routing
3. `APIExample/APIExample/Basic/` and `APIExample/APIExample/Advanced/` for implementation and API usage
4. `APIExample/APIExample/Language.h`, `APIExample/APIExample/stdafx.cpp`, `APIExample/APIExample/en.ini`, and `APIExample/APIExample/zh-cn.ini` for scene labels
5. `APIExample/APIExample/APIExample.vcxproj` for compiled source and header membership

## Procedure

1. Search the Case Index in `ARCHITECTURE.md` by case name, feature, or exact SDK API.
2. Re-scan active `m_vecBasic.push_back(...)` and `m_vecAdvanced.push_back(...)` calls in `InitSceneDialog()` before claiming that a case appears in the application. Ignore commented-out calls.
3. For API queries not answered by the index, search the matching dialog sources under both case categories and report the actual call site.
4. For a fully registered case, verify all of the following:
   - the dialog header is included and the dialog is owned by `CAPIExampleDlg`;
   - an active scene vector entry exists and its dialog is created;
   - `CreateScene()` routes the same scene label to the dialog's initialize/show path;
   - `ReleaseScene()` routes it to the cleanup/hide path;
   - the label is declared in `Language.h`, initialized from a key in `stdafx.cpp`, and present in both language files;
   - the dialog sources belong to `APIExample.vcxproj`.
5. If documentation, source, scene-list, routing, project, or localization evidence disagree, report the discrepancy instead of selecting one silently.

## Output

Report the case name, category, source files, key APIs, and registration status. For
partial or missing registration, identify the exact missing link. Keep "dialog
source exists" distinct from "case is selectable and safely released in the app."

## Never

- Never count a commented-out scene vector entry as active.
- Never use `CSceneDialog.cpp` as the registration source; this project registers cases in `APIExampleDlg`.
- Never infer registration from a folder, include, or constructed dialog alone.
- Never claim an API is demonstrated without locating its source call or an up-to-date Case Index entry.
- Never edit source, resources, project files, or documentation while using this skill.
