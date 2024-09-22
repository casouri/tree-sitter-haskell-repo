#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <tree_sitter/api.h>
#include <assert.h>

TSLanguage *tree_sitter_haskell();

static const char*
read_buffer (void *buffer, uint32_t byte_index,
		     TSPoint position, uint32_t *bytes_read) {
  if (byte_index < strlen(buffer)) {
	*bytes_read = strlen(buffer) - byte_index;
	return ((char *) buffer) + byte_index;
  }
  *bytes_read = 0;
  return NULL;
}

void log(void *payload, TSLogType log_type, const char *buffer) {
  printf ("%s\n", buffer);
}

int main() {

  TSParser *parser = ts_parser_new();
  ts_parser_set_language(parser, tree_sitter_haskell());

  /* TSLogger logger; */
  /* logger.payload = NULL; */
  /* logger.log = log; */

  /* ts_parser_set_logger(parser, logger); */

  // Initial parse

  FILE *fd = fopen("rescan.hs", "r");

  if (fd == NULL) return -1;

  char *source = NULL;
  size_t len;
  ssize_t bytes_read = getdelim(&source, &len, '\0', fd);

  if (bytes_read == -1) return -1;

  TSInput input = {(void *) source, read_buffer, TSInputEncodingUTF8};
  TSTree *tree = ts_parser_parse(parser, NULL, input);

  // Second parse

  fd = fopen("rescan2.hs", "r");
  if (fd == NULL) return -1;
  bytes_read = getdelim(&source, &len, '\0', fd);
  if (bytes_read == -1) return -1;

  TSPoint dummy_point = {0, 0};
  TSInputEdit edit = {
	7,
	19,
	7,
	dummy_point, dummy_point, dummy_point
  };

  ts_tree_edit(tree, &edit);

  TSInputEdit edit2 = {
	23,
	35,
	23,
	dummy_point, dummy_point, dummy_point
  };

  ts_tree_edit(tree, &edit2);

  input.payload = source;
  tree = ts_parser_parse(parser, tree, input);

  printf("Parse tree of edited file:\n");
  printf("%s\n", ts_node_string (ts_tree_root_node (tree)));

  // Standalone parse

  TSParser *parser2 = ts_parser_new();
  ts_parser_set_language(parser2, tree_sitter_haskell());

  tree = ts_parser_parse(parser2, NULL, input);

  printf("Parse tree of a fresh parse:\n");
  printf("%s\n", ts_node_string (ts_tree_root_node (tree)));

  return 0;
}
