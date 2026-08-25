---
name: query-cases
description: >
  Query existing macOS APIExample cases by feature name, Agora RTC API, category,
  menu label, controller identifier, or storyboard. Use before adding a case or
  when checking whether a feature is already implemented and fully registered.
  This skill is read-only and verifies live menu and storyboard wiring instead of
  relying only on folder names or the architecture index.
---

# Query Cases - macOS

## Scope

Use this skill only to inspect the main macOS `APIExample` project. It answers which
cases exist, which SDK APIs they demonstrate, where their source lives, and whether
they are reachable from the application menu. It must not edit or register cases.

## Sources Of Truth

1. `ARCHITECTURE.md` for the fast case index
2. `APIExample/ViewController.swift` for active `MenuItem` registrations
3. `APIExample/Examples/Basic/` and `APIExample/Examples/Advanced/` for implementation and API usage
4. Per-case `Base.lproj/*.storyboard` files for storyboard names and controller identifiers
5. `APIExample.xcodeproj/project.pbxproj` for target membership when registration completeness matters

## Procedure

1. Search the Case Index in `ARCHITECTURE.md` by case name, feature, or exact SDK API.
2. Re-scan the `menus` array in `APIExample/ViewController.swift` before claiming that a case is registered. Ignore commented-out `MenuItem` lines and header cells.
3. For API queries not answered by the index, search Swift sources under both example categories and report the actual call site.
4. For a fully registered case, verify all of the following:
   - an active menu item supplies a display name, `controller`, and `storyboard`;
   - the named storyboard exists under the case folder;
   - the storyboard contains the registered controller identifier;
   - the Swift source and storyboard belong to the `APIExample` Xcode target.
5. If documentation, source, menu, or storyboard evidence disagree, report the discrepancy instead of selecting one silently.

## Output

Report the case name, category, source path, key APIs, and registration status. For
partial or missing registration, identify the exact missing link. Keep "source
exists" distinct from "case is reachable in the app."

## Never

- Never count a commented-out menu item as active.
- Never infer registration from a folder or storyboard alone.
- Never claim an API is demonstrated without locating its source call or an up-to-date Case Index entry.
- Never edit source, storyboards, project files, or documentation while using this skill.
