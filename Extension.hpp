/* Extension.h
 * This is where you define your extension
 * class. Runtime variables and function
 * declarations go in here; A/C/E function
 * definitions go in their respective .cpp
 * files, other function definitions go in
 * Extension.cpp. This header is included
 * at the bottom of Common.h, so you have
 * access to everything included by it.
 */

class Extension final
{
public:
	/* rd
	 * This is a pointer to the classic
	 * RunData structure. Some things still
	 * have to be stored there for Fusion's
	 * sake, so you need to have acces to
	 * that data. You should not modify
	 * the rundata structure, however,
	 * as it is global to all extensions.
	 */
	RD *rd;

	/* rh
	 * This is a pointer to the RunHeader
	 * structure, which contains some
	 * application information.
	 */
	RunHeader *rh;

	/* Runtime
	 * This is EDIF's compendium of useful
	 * functions for every day life as an
	 * extension. You can trigger immediate
	 * events, read and write global data,
	 * etc. See the EDIF wiki for more
	 * information.
	 */
	Edif::Runtime Runtime;

	/* MinimumBuild
	 * Always set this to the latest build
	 * of Fusion that has been released, unless
	 * you need to set it higher for a beta
	 * version of Fusion to get access to some
	 * new SDK feature.
	 */
	static const int MinimumBuild = 257;

	/* Version
	 * This is the version of your extension
	 * that is stored in the editdata. You should
	 * only change this when your editdata structure
	 * changes, not just when you have a new version
	 * of your extension with new features.
	 */
	static const int Version = 1;

	/* OEFLAGS, OEPREFS
	 * These are settings for your extension
	 * that tell Fusion how to treat your extension
	 * and what behaviors and responsibilities
	 * to add. Check the MMF2SDK Help file for
	 * more detailed information on the flags you
	 * can set and what they implicate.
	 */
	static const int OEFLAGS = OEFLAG_VALUES|OEFLAG_RUNBEFOREFADEIN|OEFLAG_NEVERKILL;
	static const int OEPREFS = 0;

	/* WindowProcPriority
	 * If your extension creates a Window
	 * and implements the WindowProc function,
	 * this is the priority for that stuff.
	 */
	static const int WindowProcPriority = 100;

	Extension(RD *rd, SerializedED *SED, createObjectInfo *COB); //defined & documented in Extension.cpp
	~Extension();                                                //defined & documented in Extension.cpp


	/* Add any data you want to store in your
	 * extension to this class (eg. what you'd
	 * normally store in the rundata). Unlike
	 * other SDKs, you can store real C++
	 * objects with constructors and destructors,
	 * without having to call them manually or
	 * store a pointer.
	 */

	stdtstring error_msg;
	struct SlotId final
	{
		int id;
		stdtstring filepath;

		SlotId(int id, stdtstring const &filepath)
		: id{id}
		, filepath{filepath}
		{
		}

		bool operator<(SlotId const &other) const noexcept
		{
			return id < other.id;
		}
		bool operator==(SlotId const &other) const noexcept
		{
			return id == other.id;
		}
		friend bool operator<(SlotId const &slot, int id) noexcept
		{
			return slot.id < id;
		}
		friend bool operator<(int id, SlotId const &slot) noexcept
		{
			return id < slot.id;
		}
		friend bool operator<(SlotId const &slot, stdtstring const &filepath) noexcept
		{
			return slot.filepath < filepath;
		}
		friend bool operator<(stdtstring const &filepath, SlotId const &slot) noexcept
		{
			return filepath < slot.filepath;
		}
		friend bool operator<(SlotId const &slot, TCHAR const *filepath) noexcept
		{
			return slot.filepath < filepath;
		}
		friend bool operator<(TCHAR const *filepath, SlotId const &slot) noexcept
		{
			return filepath < slot.filepath;
		}
	};
	std::map<SlotId, std::fstream, std::less<>> slots;

	template<typename... Args>
	void generate_error(Args &&... args)
	{
		std::wostringstream message;
		using helper = int[];
		(void)helper{0, (void(message << std::forward<Args>(args)), 0)...};
		error_msg = message.str();
		Runtime.GenerateEvent(0); //OnError
	}


	/* Add your actions, conditions, and expressions
	 * as real class member functions here. The arguments
	 * (and return types for expressions/conditions)
	 * must match EXACTLY what you defined in the JSON.
	 * As a handy tip, all parameters will be either
	 * a pointer, an int, or a float (each only 4 bytes)
	 * and the same applies to return types. Remember to
	 * link the actions, conditions and expressions to their
	 * numeric IDs in the class constructor in Extension.cpp.
	 */

	//Actions - Defined in Actions.cpp
	void OpenStream(TCHAR const *filepath, int slot, int flags);
	void CloseStream(int slot);
	void SetByte(int slot, unsigned position, int value);
	void SetShort(int slot, unsigned position, int value);
	void SetInt(int slot, unsigned position, int value);
	void SetFloat(int slot, unsigned position, float value);
	void SetDouble(int slot, unsigned position, float value);
	void SetString8(int slot, unsigned position, TCHAR const *str, int nullterm);
	void SetString16(int slot, unsigned position, TCHAR const *str, int nullterm);
	void ClearError(int slot);
	void FromMemory(int slot, unsigned position, void const *memory, unsigned size);
	void ToMemory(int slot, unsigned position, void *memory, unsigned size);

	//Conditions - Defined in Conditions.cpp
	bool OnError();
	bool IsOpen(int slot);
	bool IsOk(int slot);
	bool FileExists(TCHAR const *filepath);

	//Expressions - Defined in Expressions.cpp
	TCHAR const *GetError();
	int SignedByteAt(int slot, unsigned position);
	int SignedShortAt(int slot, unsigned position);
	int UnsignedByteAt(int slot, unsigned position);
	int UnsignedShortAt(int slot, unsigned position);
	int IntAt(int slot, unsigned position);
	float FloatAt(int slot, unsigned position);
	float DoubleAt(int slot, unsigned position);
	TCHAR const *String8At(int slot, unsigned position);
	TCHAR const *String16At(int slot, unsigned position);
	unsigned ReadCursorPos(int slot);
	unsigned WriteCursorPos(int slot);
	unsigned FileSize(int slot);


	short Handle();         //defined & documented in Extension.cpp
	short Display();        //defined & documented in Extension.cpp

	short Pause();          //defined & documented in Extension.cpp
	short Continue();       //defined & documented in Extension.cpp

	bool Save(HANDLE File); //defined & documented in Extension.cpp
	bool Load(HANDLE File); //defined & documented in Extension.cpp

	//defined & documented in Extension.cpp
	void Action(int ID, RD *rd, long param1, long param2);
	long Condition(int ID, RD *rd, long param1, long param2);
	long Expression(int ID, RD *rd, long param);
};
