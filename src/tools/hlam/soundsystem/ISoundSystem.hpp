#pragma once

#include <string_view>

#undef PlaySound

/**
*	@defgroup SoundSystem OpenAL based sound system.
*
*	@{
*/

namespace filesystem
{
class IFileSystem;
}

namespace soundsystem
{
/**
*	A sound system that can be used to play back sounds. Sounds are non-looping.
*/
class ISoundSystem
{
public:
	virtual ~ISoundSystem() {};

	/**
	*	@brief Whether sound is available (i.e. is a device available)
	*/
	virtual bool IsSoundAvailable() const = 0;

	/**
	*	Initializes the sound system. Should be called on startup.
	*	@return true on success, false otherwise.
	*/
	virtual bool Initialize(filesystem::IFileSystem* filesystem) = 0;

	/**
	*	Shuts down the sound system. Should be called on shutdown.
	*/
	virtual void Shutdown() = 0;

	/**
	*	Must be called every frame.
	*/
	virtual void RunFrame() = 0;

	/**
	*	@brief Plays a sound by name. The filename is relative to the game's sound directory, and is looked up using the filesystem.
	*	@param fileName Sound filename.
	*	@param volume Volume. Expressed as a range between [0, 1].
	*	@param pitch Pitch amount. Expressed as a range between [0, 255].
	*/
	virtual void PlaySound(std::string_view fileName, float volume, int pitch) = 0;

	/**
	*	Stops all sounds that are currently playing.
	*/
	virtual void StopAllSounds() = 0;
};
}

/** @} */
