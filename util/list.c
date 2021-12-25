#include <zmonitors-util.h>

ZMS_EXPORT void
zms_list_init(struct zms_list *list)
{
  list->prev = list;
  list->next = list;
}

ZMS_EXPORT void
zms_list_insert(struct zms_list *list, struct zms_list *elm)
{
  elm->prev = list;
  elm->next = list->next;
  list->next = elm;
  elm->next->prev = elm;
}

ZMS_EXPORT void
zms_list_remove(struct zms_list *elm)
{
  elm->prev->next = elm->next;
  elm->next->prev = elm->prev;
  elm->next = NULL;
  elm->prev = NULL;
}
