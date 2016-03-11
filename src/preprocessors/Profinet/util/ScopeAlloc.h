#ifndef __SCOPE_ALLOC_H__
#define __SCOPE_ALLOC_H__

void *appScopeMalloc(size_t size);

void *packetScopeMalloc(size_t size);

void *clientScopeMalloc(size_t size);

int appScopeFree();

int packetScopeFree();

int clientScopeFree();

#endif
