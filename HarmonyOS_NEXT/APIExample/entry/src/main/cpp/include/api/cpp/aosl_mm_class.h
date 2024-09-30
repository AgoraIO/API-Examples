/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jun 17th, 2024
 * Module:	AOSL memory management for C++ definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2024 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_MM_CLASS_H__
#define __AOSL_MM_CLASS_H__


#include <api/aosl_types.h>
#include <api/aosl_mm.h>


class aosl_mm_class {
public:
	static void *operator new (size_t size)
	{
		return aosl_mm_malloc (size);
	}

	static void *operator new [] (size_t size)
	{
		return aosl_mm_malloc (size);
	}

	static void operator delete (void *ptr)
	{
		aosl_mm_free (ptr);
	}

	static void operator delete [] (void *ptr)
	{
		aosl_mm_free (ptr);
	}
};


#endif /* __AOSL_MM_CLASS_H__ */