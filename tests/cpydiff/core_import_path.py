"""
categories: Core,import
description: __path__ attribute of a package has a different type (single string instead of list of strings) in Pycopy
cause: Pycopy does't support namespace packages split across filesystem. Beyond that, Pycopy's import system is highly optimized for minimal memory usage.
workaround: Details of import handling is inherently implementation dependent. Don't rely on such details in portable applications.
"""
import modules

print(modules.__path__)
