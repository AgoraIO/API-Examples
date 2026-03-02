# How to Create a New Windows API Example

When adding a new independent example to the Windows APIExample project, follow this checklist. For naming, structure, and menu conventions, see `ARCHITECTURE.md`.

## 1. Determine Example Name and Category

- **Name:** Short PascalCase name (e.g., `MyFeature`), used for folder name and class name.
- **Category:** Place example in **Basic** or **Advanced** (`APIExample/APIExample/Basic/` or `APIExample/APIExample/Advanced/`).

## 2. Create Folder and Dialog Files

- Create folder: `APIExample/APIExample/<Basic|Advanced>/<ExampleName>/`
- Add header file: `C<ExampleName>Dlg.h`
- Add implementation file: `C<ExampleName>Dlg.cpp`

**Dialog Class (`C<ExampleName>Dlg`):**

- Inherits from `CDialogEx` or `CDialog`
- Implements `IAgoraRtcEngineEventHandler` interface
- Manages its own Agora engine lifecycle
- Defines message handlers via `BEGIN_MESSAGE_MAP` / `END_MESSAGE_MAP`
- Owns all UI controls and state for that example

**Reference implementation:** Check existing examples in `APIExample/APIExample/Basic/` and `Advanced/`


## 3. Register in Scene Dialog

- Open `APIExample/APIExample/CSceneDialog.cpp`
- Add example to the appropriate scene list (Basic or Advanced)
- Example name must match folder name

## 4. Add to Visual Studio Project

- New files must be added to the Visual Studio project
- In Visual Studio: Right-click project → "Add" → "Existing Item"
- Or update `APIExample.vcxproj` directly

## 5. Create SKILL.md API Usage Guide

Create `SKILL.md` file in the example directory to guide users on using the API in business scenarios.

**File path:** `APIExample/APIExample/<Basic|Advanced>/<ExampleName>/SKILL.md`

**Use template:** Copy `.agent/skills/create-api-example/assets/SKILL-template.md` and fill in content

**Content structure:**
1. **YAML frontmatter** - name, description, metadata
2. **Feature description** - Brief description of core functionality
3. **Core API call flow** - 5-7 key steps with code examples
4. **Key event callbacks** - Important event handler methods and their purpose
5. **Common questions** - 3-6 FAQ items with solutions
6. **Reference documentation** - Official documentation links
7. **Related examples** - References to other related examples

**Writing tips:**
- Extract key API calls from example code
- Provide complete code snippets with important parameters
- Explain each parameter's purpose and optional values
- List common questions and solutions
- Add official documentation links

## Checklist Summary

- [ ] Create example folder under `APIExample/APIExample/Basic/` or `Advanced/`
- [ ] Implement dialog class inheriting from `CDialogEx` or `CDialog`
- [ ] Implement `IAgoraRtcEngineEventHandler` interface
- [ ] Create `SKILL.md` with API usage guide
- [ ] Register example in `CSceneDialog`
- [ ] Add files to Visual Studio project
- [ ] Build and test the application

After completion, build and run the app. The new example will appear in the scene list and can be launched.
