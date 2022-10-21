/*
 * this file is part of Startup Manager
 * Copyright (C) 2004-2008 Glenn Van Loon, glenn@startupmanager.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef BTWININIEXCEPTION_H
#define BTWININIEXCEPTION_H

///	@file BtWininiException.h Declaration of the Boot::Winini::Exception class.

#include "BtException.h"

namespace Boot
{

	namespace Winini
	{

		/**
		 *	@brief An exception class.
		 *	@details This class is used for exceptions in the
		 *	Boot::Winini namespace.
		 */
		class Exception : public Boot::Exception
		{
		public:
			/**
			 *	@name Construction/Destruction
			 *	@{
			 */
			/**
			 *	@brief The constructor.
			 *	@param msg The error message.
			 */
			explicit Exception(tstring const &msg) throw();
			///	The default destructor.
			virtual ~Exception();
			/**	@} <!-- end of the group --> */
		};

		inline Exception::Exception(tstring const &msg) throw()
			: Boot::Exception(msg)
		{
		}

		inline Exception::~Exception()
		{
		}

	}	// namespace Winini

}	// namespace Boot

#endif	// BTWININIEXCEPTION_H
