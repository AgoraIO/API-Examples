# How to Create a New macOS API Example

When adding a new independent example to the macOS APIExample project, follow this checklist. For naming, structure, and menu conventions, see `ARCHITECTURE.md`.

## 1. Determine Example Name and Category

- **Name:** Short PascalCase name (e.g., `MyFeature`), used for folder name and class name.
- **Category:** Place example in **Basic** or **Advanced** (`APIExample/Examples/Basic/` or `APIExample/Examples/Advanced/`).

## 2. Create Folder and Swift File

- Create folder: `APIExample/Examples/<Basic|Advanced>/<ExampleName>/`
- Add Swift file (e.g., `MyFeature.swift`)

**Example Class (`<ExampleName>`):**

- Manages its own Agora engine lifecycle
- Implements `AgoraRtcEngineDelegate`
- Receives configuration via initialization or property injection
- Owns all UI elements for that example

**Reference implementation:** Check existing examples in `APIExample/Examples/`

## 3. Create Storyboard (if needed)

- Create file: `APIExample/Examples/<Basic|Advanced>/<ExampleName>/Base.lproj/<ExampleName>.storyboard`
- Add UI layout for the example
- Connect outlets and actions to the example class

## 4. Register in Menu

- Open `APIExample/ViewController.swift`
- Add example to the appropriate menu section
- Example name must match folder name

## 5. Add to Xcode Target

- New files must be added to the app target's **Resources** and **Compile Sources**
- In Xcode: Right-click example folder → "Add Files to Target"
- Or update `APIExample.xcodeproj/project.pbxproj` directly

## 6. Create SKILL.md API Usage Guide

Create `SKILL.md` file in the example directory to guide users on using the API in business scenarios.

**File path:** `APIExample/Examples/<Basic|Advanced>/<ExampleName>/SKILL.md`

**Use template:** Copy `.agent/skills/create-api-example/assets/SKILL-template.md` and fill in content

**Content structure:**
1. **YAML frontmatter** - name, description, metadata
2. **Feature description** - Brief description of core functionality
3. **Core API call flow** - 5-7 key steps with code examples
4. **Key event callbacks** - Important delegate methods and their purpose
5. **Common questions** - 3-6 FAQ items with solutions
6. **Reference documentation** - Official documentation links
7. **Related examples** - References to other related examples

**Reference example:** Check existing `SKILL.md` files in the project

**Writing tips:**
- Extract key API calls from example code
- Provide complete code snippets with important parameters
- Explain each parameter's purpose and optional values
- List common questions and solutions
- Add official documentation links

## Checklist Summary

- [ ] Create example folder under `APIExample/Examples/Basic/` or `Advanced/`
- [ ] Implement example class implementing `AgoraRtcEngineDelegate`
- [ ] Create storyboard (if needed)
- [ ] Create `SKILL.md` with API usage guide
- [ ] Register example in `ViewController.swift`
- [ ] Add files to Xcode target
- [ ] Build and test the application

After completion, build and run the app. The new example will appear in the menu and can be launched.


## 3. Create Storyboard (if needed)

- Create file: `APIExample/Examples/<Basic|Advanced>/<ExampleName>/Base.lproj/<ExampleName>.storyboard`
- Add UI layout for the example
- Connect outlets and actions to the example class

## 4. Register in Menu

- Open `APIExample/ViewController.swift`
- Add example to the appropriate menu section
- Example name must match folder name

## 5. Add to Xcode Target

- New files must be added to the app target's **Resources** and **Compile Sources**
- In Xcode: Right-click example folder → "Add Files to Target"
- Or update `APIExample.xcodeproj/project.pbxproj` directly

## 6. Create SKILL.md API Usage Guide

Create `SKILL.md` file in the example directory to guide users on using the API in business scenarios.

**File path:** `APIExample/Examples/<Basic|Advanced>/<ExampleName>/SKILL.md`

**Use template:** Copy `.agent/skills/create-api-example/assets/SKILL-template.md` and fill in content

**Content structure:**
1. **YAML frontmatter** - name, description, metadata
2. **Feature description** - Brief description of core functionality
3. **Core API call flow** - 5-7 key steps with code examples
4. **Key event callbacks** - Important delegate methods and their purpose
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

- [ ] Create example folder under `APIExample/Examples/Basic/` or `Advanced/`
- [ ] Implement example class implementing `AgoraRtcEngineDelegate`
- [ ] Create storyboard (if needed)
- [ ] Create `SKILL.md` with API usage guide
- [ ] Register example in `ViewController.swift`
- [ ] Add files to Xcode target
- [ ] Build and test the application

After completion, build and run the app. The new example will appear in the menu and can be launched.
