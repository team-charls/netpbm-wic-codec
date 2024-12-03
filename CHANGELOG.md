<!--
  SPDX-FileCopyrightText: © 2024 Team CharLS
  SPDX-License-Identifier: BSD-3-Clause
-->

# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/) and this project adheres to [Semantic Versioning](http://semver.org/).

## [0.3.0 - 2024-10-17]

### Added

- Added property store to display image properties in Explorer preview pane, details tab sheet and info tip.
- Added custom action in installer to ensure SHChangeNotify is triggered and Windows Explorer is aware of the changes.
- Added SourceLink NuGet package to ensure links to the source code are included in the symbols file.

### Changed

- Reduced binary size of the DLL from 288 Kbyte to 65 Kbyte (excluded usage of std::format).
- Enabled reproducible release builds.

## [0.2.0 - 2024-10-8]

### Added

- Initial release.
