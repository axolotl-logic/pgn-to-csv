#include "pgn_tag.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "common/smalloc.h"
#include "tagspec.h"

tag_t *tag_new(const char *name, const char *value, tag_t *next) {
    tag_t *tag = smalloc(sizeof(*tag));

    tag->name = strdup(name);
    if (value != NULL) {
        tag->value = strdup(value);
    } else {
        tag->value = NULL;
    }

    tag->next = next;

    return tag;
}

void taglist_add(taglist_t *tags, const char *name, const char *value) {
    tag_t *tag = tag_new(name, value, NULL);
    if (tags->tail == NULL) {
        tags->tail = tag;
        tags->head = tag;
    } else {
        tags->tail->next = tag;
        tags->tail = tags->tail->next;
    }
}

taglist_t *taglist_new(void) {
    taglist_t *tags = smalloc(sizeof(*tags));

    return tags;
}

void tag_free(tag_t *tag) {
    while (tag != NULL) {
        tag_t *next = tag->next;

        free(tag->name);
        free(tag->value);
        free(tag);

        tag = next;
    }
}

void taglist_free(taglist_t *tags) {
    if (tags == NULL) {
        return;
    }

    tag_free(tags->head);

    free(tags);
}

taglist_t *taglist_new_aligned(taglist_t *tags, tagspec_t *spec) {
    taglist_t *aligned_tags = taglist_new();

    for (tagorder_t *order = spec->order_head; order != NULL;
         order = order->next) {
        // Find the tag corresponding to the comparior
        bool found = false;
        for (tag_t *tag = tags->head; tag != NULL; tag = tag->next) {
            if (strcmp(order->name, tag->name) == 0) {
                taglist_add(aligned_tags, tag->name, tag->value);
                found = true;
                break;
            }
        }

        if (!found) {
            taglist_add(aligned_tags, order->name, NULL);
        }
    }

    return aligned_tags;
}
