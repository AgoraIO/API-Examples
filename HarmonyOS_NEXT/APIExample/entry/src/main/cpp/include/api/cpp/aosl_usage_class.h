/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jan 6th, 2019
 * Module:	AOSL simple usage object for C++ definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2019 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_USAGE_OBJ_CPP_H__
#define __AOSL_USAGE_OBJ_CPP_H__


#include <stdlib.h>

#include <api/aosl_types.h>
#include <api/aosl_atomic.h>


class aosl_usage_class {
private:
	bool atomic;
	int usage;

protected:
	/* We do not allow delete this object directly. */
	virtual ~aosl_usage_class () {}

public:
	/**
	 * The parameter 'atomic_op' indicates whether the operations
	 * on the usage should be atomic operations, true for atomic,
	 * and false for non-atomic. The default value is false.
	 **/
	aosl_usage_class (bool atomic_op = false)
	{
		atomic = atomic_op;

		if (atomic_op) {
			aosl_atomic_set ((aosl_atomic_t *)&usage, 1);
		} else {
			usage = 1;
		}
	}

	void inc_ref ()
	{
		if (atomic) {
			aosl_atomic_inc ((aosl_atomic_t *)&usage);
		} else {
			usage++;
		}
	}

	/**
	 * Return values:
	 *       true: the usage count reached zero, and deleted 'this' object already;
	 *      false: the usage count still greater than zero, 'this' object is still alive;
	 **/
	bool dec_ref ()
	{
		bool b_val;

		if (atomic) {
			b_val = (bool)!!aosl_atomic_dec_and_test ((aosl_atomic_t *)&usage);
		} else {
			usage--;
			b_val = (bool)(usage == 0);
		}

		if (b_val) {
			delete this;
			return true;
		}

		return false;
	}
};


#endif /* __AOSL_USAGE_OBJ_CPP_H__ */