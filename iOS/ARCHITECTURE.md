# iOS ARCHITECTURE

This document describes the top-level structure of the iOS directory.

## Directory Structure

```
iOS/
├── APIExample/              # UIKit + Swift implementation
│   ├── AGENTS.md            # Agent guide for this project
│   └── ARCHITECTURE.md      # Architecture rules for this project
├── APIExample-SwiftUI/      # SwiftUI implementation
│   ├── AGENTS.md            # Agent guide for this project
│   └── ARCHITECTURE.md      # Architecture rules for this project
├── APIExample-OC/           # Objective-C implementation
│   ├── AGENTS.md            # Agent guide for this project
│   └── ARCHITECTURE.md      # Architecture rules for this project
├── APIExample-Audio/        # Audio-focused implementation
│   ├── AGENTS.md            # Agent guide for this project
│   └── ARCHITECTURE.md      # Architecture rules for this project
├── APIExample.xcodeproj/    # Shared Xcode project
├── .agent/                  # Agent skills and utilities
│   └── skills/
│       ├── create-api-example/
│       ├── find-api-example/
│       └── migrate-api-to-project/
├── AGENTS.md                # Agent guide for this directory
└── ARCHITECTURE.md          # This file
```

Each project folder contains its own `ARCHITECTURE.md` describing its internal structure and rules, and an `AGENTS.md` guiding agents on how to work within that project.
