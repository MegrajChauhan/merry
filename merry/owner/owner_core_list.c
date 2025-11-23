#include <owner_core_list.h>

OwnerCoreList* owner_core_list_create(size_t cap) { 
	if (cap == 0)                                                    
		return NULL;                                     
	OwnerCoreList *lst = (OwnerCoreList *)malloc(sizeof(OwnerCoreList));        
 	if (!(lst)) {               
 		FATAL("Failed to initialize owner Core list", NULL);                                    
		return NULL;                                     
    }                                                              
    (lst)->buf = (OwnerCore *)calloc(cap, sizeof(OwnerCore));       
    if (!(lst)->buf) {                                            
      free(lst);
      FATAL("Failed to initialize owner Core list buffer", NULL);                                                  
      return NULL;                                     
    }                                                              
    (lst)->cap = cap;                                             
    (lst)->curr_ind = 0;                                          
    return lst;                                                       
}                                                                           

void owner_core_list_destroy(OwnerCoreList *lst) {                                                                         
    free(lst->buf);                                                            
    free(lst);                                                                 
}                                                                            

bool owner_core_list_push(OwnerCoreList *lst, OwnerCore *elem) {     
    if (lst->curr_ind >= lst->cap)                                             
      return false;                                                   
    lst->buf[lst->curr_ind] = *elem;                                           
    lst->curr_ind++;                                                           
    return true;                                                       
}                                                                            

bool owner_core_list_pop(OwnerCoreList *lst, OwnerCore *elem) {                                                
    if (lst->curr_ind == 0)                                                    
      return false;                                                  
    lst->curr_ind--;                                                           
    *elem = lst->buf[lst->curr_ind];                                           
    return true;                                                       
}                                                                            

bool owner_core_list_at(OwnerCoreList *lst, OwnerCore *elem,         
                                   size_t ind) {                              
    if (ind >= lst->cap)                                                       
      return false;                                                  
    *elem = lst->buf[ind];                                                     
    return true;                                                       
}                                                                                                                                                        

bool owner_core_list_resize(OwnerCoreList *lst,                 
                                       size_t resize_factor) {                
    socket_t *new_buf = (socket_t *)malloc(sizeof(socket_t) * lst->cap * resize_factor);   
    if (!new_buf) {
      FATAL("Failed to resize owner Core list", NULL);
      return false;                                                 
    }                                                              
    mempcpy(new_buf, lst->buf, lst->curr_ind * sizeof(socket_t));              
    socket_t *tmp = lst->buf;                                                  
    lst->buf = new_buf;                                                        
    lst->cap *= resize_factor;                                                 
    free(tmp);                                                                 
    return true;                                                       
}                                                                            

size_t owner_core_list_size(                      
    OwnerCoreList *lst) {                                                
    return lst->curr_ind;                                                      
}                                                                            

size_t owner_core_list_index_of(                  
    OwnerCoreList *lst, OwnerCore *elem) {                                    
    return (size_t)(elem - lst->buf);      
}
