
#include "libMacEmu.h"
#include "vector_array.h"
#include "proto/gadtools.h"	// used by AmigaOS menu system.

// I expected to run into name conflict, m macro is like namespace.



extern struct _vector_array * m(windows) ;

struct NewMenu *n(menu) = NULL;
int m(menus_items_count) = 0;
int n(create_menu_counter) = 0;

struct _vector_array *m(menus) = NULL ;

	// set amiga menu to all windows...

void n(attach_menu_to_window)(void *item)
{

	if (item == NULL)
	{
		printf("critical error, no item found\n...");
		getchar();
		return ;
	}

	n(AWC) *awc = &((GrafPort *) (item))->AmigaWindowContext;

	if (awc)
	{

		if (awc -> win == NULL) printf("wtf... amiga window missing\n");

		awc -> vi= (struct VisualInfo *) GetVisualInfoA( awc -> win -> WScreen,  NULL);
		awc -> menu = CreateMenus( n(menu) );

		if ((awc -> vi) && (awc-> menu))
		{
			if ( awc -> vi ) LayoutMenus( awc -> menu, awc -> vi, GTMN_NewLookMenus, TRUE, TAG_END );
		}
		SetMenuStrip(awc -> win, awc -> menu );
	}
	else
	{
		printf("no awc unexpected\n");
	}
}

void n(attach_menu)()
{
	printf("%s:%d\n",__FUNCTION__,__LINE__);
	if (n(menu) == NULL) return;	
	_vector_array_for_each( m(windows) , n(attach_menu_to_window) );
}

void n(detach_menu_from_window) (void *item)
{
	n(AWC) *awc = &((GrafPort *) (item)) -> AmigaWindowContext;

	if (awc)
	{
		if (awc -> menu)
		{
			if (awc -> win) ClearMenuStrip(awc -> win);
			 FreeMenus(awc-> menu);
		}

		if (awc -> vi) FreeVisualInfo(awc -> vi);
		awc -> menu = NULL;
		awc -> vi =  NULL;
	}
	else
	{
		printf("no awc unexpected\n");
	}
}

void n(detach_menu)()
{
	printf("%s:%d\n",__FUNCTION__,__LINE__);
	if (n(menu) == NULL) return;	
	_vector_array_for_each( m(windows) , n(detach_menu_from_window) );
}

void n(free_menu)()
{
	if (n(menu))
	{
		FreeMenus( ( struct Menu *) n(menu) );
		n(menu) = NULL;
	}
}

#define menuID( menu, item )  (void *) ((menu <<16) | item)

void m(set_menus_items)(void *item)		// We need the size, so we can make menu table for AmigaOS.
{
	int n = n(create_menu_counter);
	int flag = 0;
	int nn;

	__tmp_mac_menu__ *menu = (__tmp_mac_menu__ *) item;

	n(menu)[n].nm_Type = NM_TITLE;
	n(menu)[n].nm_Label = (STRPTR) menu -> description;
    	n(menu)[n].nm_CommKey = "";
    	n(menu)[n].nm_Flags = flag;        
    	n(menu)[n].nm_MutualExclude = 0 ;
    	n(menu)[n].nm_UserData = menuID( menu -> id ,0 );

	n(create_menu_counter) ++;
	n = n(create_menu_counter);

	for (nn = 0;nn < menu -> items_count ; nn++)
	{
		n(menu)[n].nm_Type = NM_ITEM;
		n(menu)[n].nm_Label = (STRPTR) menu -> items[nn].name;
   	 	n(menu)[n].nm_CommKey = menu -> items[nn].key;
   	 	n(menu)[n].nm_Flags = menu -> items[nn].flags;        
   	 	n(menu)[n].nm_MutualExclude = 0 ;
	    	n(menu)[n].nm_UserData = menuID( menu -> id ,nn+1 );

		n(create_menu_counter) ++;
		n = n(create_menu_counter);
	}

	n(menu)[n].nm_Type =NM_END;	// set end of menu		(will be overwritten... for next menu :-) )
	n(menu)[n].nm_Label = NULL;

}

void m(get_menus_items_count)(void *item)		// We need the size, so we can make menu table for AmigaOS.
{
	__tmp_mac_menu__ *menu = (__tmp_mac_menu__ *) item;
	m(menus_items_count) += menu -> items_count + 1;	// title and items.
}

void n(create_new_menu)()
{
	m(menus_items_count) = 0;
	_vector_array_for_each( m(menus) , m(get_menus_items_count) );

	if (n(menu)) FreeVec(n(menu));
	n(menu) = (struct NewMenu *) AllocVecTags( sizeof(struct NewMenu) * (m(menus_items_count) + 1), TAG_END );

	if (n(menu))
	{
		n(create_menu_counter) = 0;
		_vector_array_for_each( m(menus) , m(set_menus_items) );
	}
}

void n(create_menu)()
{
	if (m(menus) == NULL) return;	

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	n(detach_menu)();
	n(free_menu)();
	n(create_new_menu)();
	n(attach_menu)();
}

void HiliteMenu()
{
//	printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
}

void DrawMenuBar()
{
	printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
}

MenuHandle NewMenu(short id, const char *description)
{
	__tmp_mac_menu__ *menu;		// tmp, I don't know correct structure for the menu yet.

	menu = (__tmp_mac_menu__ *) calloc (1,sizeof(__tmp_mac_menu__));

	if (menu)
	{
		menu -> id = id;
		menu -> description = strdup(description);
		menu -> items = (_tmp_mac_menu_item_ *) malloc(sizeof(_tmp_mac_menu_item_) * 100 ); // just a array of strings this might change.
	}
	return menu;
}

void free__tmp_mac_menu_item_( _tmp_mac_menu_item_  *item)
{
	if (item -> name)
	{
		if (item -> name != NM_BARLABEL) free(item -> name);
	}
	if (item -> key) free(item -> key);
}

void dispose_menu (void *item)
{
	__tmp_mac_menu__ *menu = (__tmp_mac_menu__ *) item;

	if (menu)
	{
		if (menu -> items) 
		{
			int n = 0;
			for (n=0; n<menu -> items_count;n++ )
			{
				free__tmp_mac_menu_item_( &menu -> items[n] );
			}

			free (menu -> items);
		}
		free(menu);
	}
}

void set__tmp_mac_menu_item_(_tmp_mac_menu_item_ *item ,  char *name,  char *key)
{
	item -> flags = 0;

	if (strcmp(name,"(-") == 0)
	{
		free(name);
		name = NM_BARLABEL;
	}

	item -> name = name;
	item -> key = key;
}

void __append_menu_item(MenuHandle menu, const char *start_ptr, const char *key_ptr, const char *end_ptr)
{
	if (start_ptr == end_ptr) return;

	if (menu -> items)
	{
		if (key_ptr)
		{
			set__tmp_mac_menu_item_(
				&menu -> items[ menu -> items_count ],
				strndup( start_ptr, (int) (key_ptr - start_ptr - 1) ),
				strndup( key_ptr, (int) (end_ptr - key_ptr) )
				);
		}	
		else
		{
			set__tmp_mac_menu_item_(
				&menu -> items[ menu -> items_count ],
				strndup(start_ptr, (int) (end_ptr - start_ptr)),
				NULL);
		}

		menu -> items_count ++;
	}
}

void AppendMenu(MenuHandle menu, const char *cvs_items)
{
	const char *last_ptr = cvs_items;
	const char *ptr;
	const char *key_ptr = 0;

	printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);

	for (ptr = last_ptr; *ptr ; ptr++)
	{
		switch (*ptr)
		{
			case '/': // hotkey...
				key_ptr = ptr +1;	// next after symbol.
				break;					

			case ';':
				__append_menu_item( menu, last_ptr, key_ptr, ptr);
				last_ptr = ptr +1;	// next after symbol.
				key_ptr = 0;
				break;
		}
	}

	__append_menu_item( menu, last_ptr, key_ptr, ptr);

	n(create_menu)();
}

void InsertMenu( MenuHandle menu, short num )
{
	printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);

	if (m(menus) == NULL)
	{
		m(menus) = _vector_array_new( dispose_menu );
	}

	if (m(menus))
	{
		_vector_array_push_back(m(menus),  (void  *) menu );
	}

	n(create_menu)();
}

void InitMenus()
{
	printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
}

short MenuKey(char key) 
{
	printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	return 0;
}

uint32_t MenuSelect(m(where) where)
{
	printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	return where.code;
}

void AddResMenu(MenuHandle menu, uint16_t ref)	// ref is 4 char id.
{
	printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
}

void CheckItem(MenuHandle menu, int width, bool enabled)
{
	printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
}

void	EnableItem(MenuHandle menu, short item)
{
	printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);

	if (item) menu -> items[item-1].flags &= ~NM_ITEMDISABLED;
	n(refresh_menu) = true;
}

void 	DisableItem(MenuHandle menu, short item)
{
	printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);

	if (item) menu -> items[item-1].flags |= NM_ITEMDISABLED;
	n(refresh_menu) = true;
}

void GetItem( MenuHandle menu, int Item, char *name)
{
	printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);

	sprintf(name,"%s","helloWorld");
}

