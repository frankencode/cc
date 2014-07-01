/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/Singleton.h>
#include "InetAddressSyntax.h"

namespace flux {
namespace net {

InetAddressSyntax::InetAddressSyntax(SyntaxDebugFactory *debugFactory)
{
	DEFINE("DecimalOctet",
		REPEAT(0, 3, RANGE('0', '9'))
	);

	DEFINE("HexWord",
		REPEAT(1, 4,
			CHOICE(
				RANGE('0', '9'),
				RANGE('a', 'f'),
				RANGE('A', 'F')
			)
		)
	);

	inet4Address_ =
		DEFINE("Inet4Address",
			GLUE(
				INLINE("DecimalOctet"),
				CHAR('.'),
				INLINE("DecimalOctet"),
				CHAR('.'),
				INLINE("DecimalOctet"),
				CHAR('.'),
				INLINE("DecimalOctet")
			)
		);

	inet6Address_ =
		DEFINE("Inet6Address",
			GLUE(
				REPEAT(1, 7,
					GLUE(
						CHOICE(
							INLINE("HexWord"),
							CHAR(':')
						),
						CHAR(':')
					)
				),
				CHOICE(
					REF("Inet4Address"),
					INLINE("HexWord")
				)
			)
		);

	inetAddress_ =
		DEFINE("InetAddress",
			CHOICE(
				REF("Inet4Address"),
				REF("Inet6Address")
			)
		);

	ENTRY("InetAddress");
	LINK();
}

const InetAddressSyntax *inetAddressSyntax() { return Singleton<InetAddressSyntax>::instance(); }

}} // namespace flux::net