# Contributing to JQBSTM32 Framework

Thank you for your interest in contributing to the development of JQBSTM32 Framework!

## License and Copyright

JQBSTM32 Framework is licensed under the GNU General Public License v3.0 or later. By submitting code to this project, you accept the following terms:

### Contribution Requirements

1. **GPL v3.0 Compliance**: All contributions must be compatible with GPL v3.0 license
2. **Copyright**: You retain copyright to your code, but license it under GPL v3.0 terms
3. **Copyright headers**: Each new file must contain appropriate copyright header

### Copyright Header Template for New Files

```cpp
/*
 * JQBSTM32 Framework - [File Description]
 * Copyright (C) [Year] [Your Name] ([email])
 * Copyright (C) 2024 JAQUBA (kjakubowski0492@gmail.com)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
```

## How to Contribute

### Reporting Bugs

1. Check if the bug hasn't already been reported in the Issues section
2. Create a new Issue with:
   - Clear description of the problem
   - Steps to reproduce
   - Environment information (STM32, framework version)
   - Logs/error messages

### Proposing New Features

1. Open an Issue with "feature request" tag
2. Describe:
   - Purpose of the new feature
   - Proposed API
   - Use cases
   - Impact on existing code

### Pull Requests

1. **Fork** this repository
2. Create a **feature branch** (`git checkout -b feature/AmazingFeature`)
3. **Commit** your changes with descriptive messages
4. **Add tests** for new features (if applicable)
5. **Make sure** the code passes all tests
6. **Push** to your fork (`git push origin feature/AmazingFeature`)
7. Open a **Pull Request**

### Pull Request Requirements

- [ ] Code has appropriate copyright headers
- [ ] New features are documented
- [ ] Code follows project style
- [ ] All tests pass
- [ ] Added tests for new features (if applicable)
- [ ] Updated README.md (if applicable)

## Style Guide

### C/C++ Code Style

- Use 4 spaces for indentation (no tabs)
- Curly braces on new line for functions and classes
- Class names in PascalCase (`MyClass`)
- Function and variable names in camelCase (`myFunction`, `myVariable`)
- Constants in UPPER_CASE (`MAX_BUFFER_SIZE`)
- Use meaningful variable and function names

### Example:

```cpp
class ExampleClass 
{
public:
    ExampleClass();
    void myFunction(int parameterName);
    
private:
    int memberVariable;
    static const int MAX_BUFFER_SIZE = 256;
};
```

## Technical Support

- **Email**: kjakubowski0492@gmail.com
- **GitHub Issues**: For bug reports and feature requests
- **Documentation**: Check README.md and docs/ folder

## Contribution Recognition

All contributors will be listed in the ACKNOWLEDGMENTS file and in release notes.

## Important Legal Notes

- Do not submit copyrighted code without proper license
- Do not use code from licenses incompatible with GPL v3.0
- In case of license doubts, contact the maintainer

---

Thank you for contributing to the development of JQBSTM32 Framework!
