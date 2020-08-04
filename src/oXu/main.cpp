// Copyright (c) Olaru Alexandru <olarualexandru404@gmail.com>
// Licensed under the MIT license found in the LICENSE file in the root of this repository.

#include<oXu/core/game.hpp>

int main(int argc, char **argv)
{
	oxu::Game game;
	if(!game.init())
	{
		game.clean();
		return oxu::Status::code;
	}

	game.loop();

	game.clean();

	return oxu::Status::code;
}
