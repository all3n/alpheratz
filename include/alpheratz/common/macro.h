#pragma once
/**
  @author all3n
  macro definitions
 */
#undef ALPHERATZ_DELETE_FUNCTION
#undef ALPHERATZ_DISALLOW_COPY
#undef ALPHERATZ_DISALLOW_ASSIGN
#undef ALPHERATZ_DISALLOW_COPY_AND_ASSIGN
#undef ALPHERATZ_DISALLOW_IMPLICIT_CONSTRUCTORS

#define ALPHERATZ_DELETE_FUNCTION(decl) decl = delete

// Put this in the private: declarations for a class to be uncopyable.
#define ALPHERATZ_DISALLOW_COPY(TypeName) ALPHERATZ_DELETE_FUNCTION(TypeName(const TypeName&))

// Put this in the private: declarations for a class to be unassignable.
#define ALPHERATZ_DISALLOW_ASSIGN(TypeName) \
    ALPHERATZ_DELETE_FUNCTION(void operator=(const TypeName&))

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define ALPHERATZ_DISALLOW_COPY_AND_ASSIGN(TypeName)      \
    ALPHERATZ_DELETE_FUNCTION(TypeName(const TypeName&)); \
    ALPHERATZ_DELETE_FUNCTION(void operator=(const TypeName&))

#define ALPHERATZ_DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
    ALPHERATZ_DELETE_FUNCTION(TypeName());                 \
    ALPHERATZ_DISALLOW_COPY_AND_ASSIGN(TypeName)
