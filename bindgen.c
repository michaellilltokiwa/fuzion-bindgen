#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <clang-c/Index.h>

char* replace_char(char* str, char find, char replace){
    char *current_pos = strchr(str,find);
    while (current_pos) {
        *current_pos = replace;
        current_pos = strchr(current_pos,find);
    }
    return str;
}

char* mangle(char* str){
  return replace_char(replace_char(str, ' ', '_'), '*', 'p');
}

enum CXChildVisitResult visit(CXCursor c, CXCursor parent, CXClientData client_data)
{
  enum CXCursorKind cursor_kind = clang_getCursorKind(c);
  switch (cursor_kind)
  {
      case CXCursor_FunctionDecl:
        printf("    ");
        printf("%s", clang_getCString((clang_getCursorSpelling(c))));
        printf("(");
        clang_visitChildren(
            c,
            visit,
            NULL
            );
        printf(") ");
        printf("%s", clang_getCString((clang_getTypeSpelling(clang_getResultType(clang_getCursorType(c))))));
        printf(" is intrinsic\n");
        break;
      case CXCursor_ParmDecl:
        printf("%s", clang_getCString((clang_getCursorSpelling(c))));
        printf(" ");
        printf("%s, ", clang_getCString((clang_getTypeSpelling(clang_getCursorType(c)))));
        break;
      case CXCursor_StructDecl:
        printf("    %s is\n", clang_getCString((clang_getCursorSpelling(c))));
        clang_visitChildren(
            c,
            visit,
            NULL
            );
        break;
      case CXCursor_FieldDecl:
        printf("      %s", clang_getCString((clang_getCursorSpelling(c))));
        printf(" ");
        printf("%s is\n", mangle(clang_getCString((clang_getTypeSpelling(clang_getCursorType(c))))));
        break;
      default:
        printf("NYI: ");
        printf("%s", clang_getCString(clang_getCursorKindSpelling(clang_getCursorKind(c))));
        exit(1);
  }
  return CXChildVisit_Continue;
}

int main(int argc, char **argv) {
  CXIndex index = clang_createIndex(0, 0);
  CXTranslationUnit unit = clang_parseTranslationUnit(
    index,
    argv[1], NULL, 0,
    NULL, 0,
    CXTranslationUnit_None);

  if (unit == NULL)
  {
    printf("Parsing failed");
    exit(1);
  }

  printf("c is\n\n");
  printf("  int:i32 is\n");
  printf("  const_char_p:u32 is\n");
  printf("  unsigned_long:u64 is\n");
  printf("  void:unit is\n\n");
  printf("  %s is\n", replace_char(argv[1], '.', '_'));

  CXCursor cursor = clang_getTranslationUnitCursor(unit);
  clang_visitChildren(
    cursor,
    visit,
    NULL);

  clang_disposeTranslationUnit(unit);
  clang_disposeIndex(index);

   return 0;
}