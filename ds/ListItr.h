#ifndef __LIST_ITR_H__
#define __LIST_ITR_H__

#include "List.h"


/** 
 * @brief Module for iteators on double linked list. 
 * @warning The following functions should only receive iterators previously created by ListItrBegin and ListItrEnd functions.
 * @author Author Ksenia Spitsin
 */
 
 typedef struct ListItr* ListItr;
 extern ListItr g_wrongItr;

/** 
 * @brief Get itertator to the list's beginning
 *
 * @params _list : list to return begin iterator, pointing at first element
 *                 or at the end if list is empty
 * @return iterator pointing at the list's beginning.
 * @warning If _list NULL returns NULL
 */
ListItr ListItrBegin(const List* _list);

/** 
 * @brief Get itertator to the list end
 *
 * @params _list : list to return end iterator, pointing at last element
 * @return an iterator pointing at the list's end
 * @warning If _list NULL returns NULL
 */
ListItr ListItrEnd(const List* _list);

/** 
 * @brief retruns SAME_NODE if _a and _b iterators refer to same node, DIFFERENT_NODE otherwise.
 * @warning might be implemented as a macro
 */
int ListItrEquals(const ListItr _a, const ListItr _b);

/** 
 * @brief Get itertator to the next element from a given iterator
 * @warning if _itr is end iterator or NULL it will be returned
 */
ListItr ListItrNext(ListItr _itr);

/** 
 * @brief Get itertator to the previous element
 * @warning if _itr is begin iterator or NULL it will be returned
 */
ListItr ListItrPrev(ListItr _itr);

/** 
 * @brief Get data from the list node the iterator is pointing to
 *
 * @params _itr : A list iterator
 * @return the data the iterator is pointing at or NULL if pointing to the end or begining or NULL
 */
void* ListItrGet(ListItr _itr);

/** 
 * @brief Set data at the node where the iterator is pointing at
 * @return the data from the node to be changed
 */
void* ListItrSet(ListItr _itr, void* _element);

/** 
 * @brief Inserts a new node before node the iterator is pointing at
 * @return an iterator pointing at the element inserted or g_wrongItr on error.
 *
 * @warning if _itr tail or _element NULL, return g_wrongItr
 */
ListItr ListItrInsertBefore(ListItr _itr, void* _element);

/** 
 * @brief Removes the node the iterator is pointing at
 *
 * @params _itr : A list iterator
 * @return the removed data
 *
 * @warning if _itr g_wrongItr or Head/Tail returns NULL
 */
void* ListItrRemove(ListItr _itr);

#endif /*__LIST_ITR_H__*/
