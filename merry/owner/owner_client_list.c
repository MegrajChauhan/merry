#include <owner_client_list.h>

OwnerClientList* owner_client_list_create(size_t cap) { 
	if (cap == 0)                                                    
		return NULL;                                     
	OwnerClientList *lst = (OwnerClientList *)malloc(sizeof(OwnerClientList));        
 	if (!(lst)) {               
 		FATAL("Failed to initialize owner client list", NULL);                                    
		return NULL;                                     
    }                                                              
    (lst)->buf = (OwnerClient *)calloc(cap, sizeof(OwnerClient));       
    if (!(lst)->buf) {                                            
      free(lst);
      FATAL("Failed to initialize owner client list buffer", NULL);                                                  
      return NULL;                                     
    }                                                              
    (lst)->cap = cap;                                             
    (lst)->curr_ind = 0;                                          
    return lst;                                                       
}                                                                           

void owner_client_list_destroy(OwnerClientList *lst) {                                                                         
    free(lst->buf);                                                            
    free(lst);                                                                 
}                                                                            

bool owner_client_list_push(OwnerClientList *lst, OwnerClient *elem) {     
    if (lst->curr_ind >= lst->cap)                                             
      return false;                                                   
    lst->buf[lst->curr_ind] = *elem;                                           
    lst->curr_ind++;                                                           
    return true;                                                       
}                                                                            

bool owner_client_list_pop(OwnerClientList *lst, OwnerClient *elem) {                                                
    if (lst->curr_ind == 0)                                                    
      return false;                                                  
    lst->curr_ind--;                                                           
    *elem = lst->buf[lst->curr_ind];                                           
    return true;                                                       
}                                                                            

bool owner_client_list_at(OwnerClientList *lst, OwnerClient *elem,         
                                   size_t ind) {                              
    if (ind >= lst->cap)                                                       
      return false;                                                  
    *elem = lst->buf[ind];                                                     
    return true;                                                       
}                                                                                                                                                        

bool owner_client_list_resize(OwnerClientList *lst,                 
                                       size_t resize_factor) {                
    socket_t *new_buf = (socket_t *)malloc(sizeof(socket_t) * lst->cap * resize_factor);   
    if (!new_buf) {
      FATAL("Failed to resize owner client list", NULL);
      return false;                                                 
    }                                                              
    mempcpy(new_buf, lst->buf, lst->curr_ind * sizeof(socket_t));              
    socket_t *tmp = lst->buf;                                                  
    lst->buf = new_buf;                                                        
    lst->cap *= resize_factor;                                                 
    free(tmp);                                                                 
    return true;                                                       
}                                                                            

size_t owner_client_list_size(                      
    OwnerClientList *lst) {                                                
    return lst->curr_ind;                                                      
}                                                                            

size_t owner_client_list_index_of(                  
    OwnerClientList *lst, OwnerClient *elem) {                                    
    return (size_t)(elem - lst->buf);      
}
