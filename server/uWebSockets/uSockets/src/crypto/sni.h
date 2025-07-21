#ifndef SNI_H
#define SNI_H

struct us_sni_tree;

struct us_sni_tree *sni_new();
void sni_free(struct us_sni_tree *tree);
void sni_add(struct us_sni_tree *tree, const char *hostname, void *user);
void *sni_find(struct us_sni_tree *tree, const char *hostname);
void sni_remove(struct us_sni_tree *tree, const char *hostname);

#endif
