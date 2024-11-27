#include "serializer.h"
#include "robloxOpcodes.h"

struct LoadS
{
	const char* s;
	size_t size;
};

static const char* getS(lua_State* L, void* ud, size_t* size)
{
	LoadS* ls = (LoadS*)ud;
	(void)L;
	if (ls->size == 0) return NULL;
	*size = ls->size;
	ls->size = 0;
	return ls->s;
}

inline auto compress(std::string data) {

	std::string output{ "RSB1" };

	std::size_t data_size = data.size();

	std::size_t max_size = LZ4_compressBound(data_size);
	std::vector<char> compressed(max_size);

	std::size_t compression_size = LZ4_compress_default(data.data(), &compressed[0], data_size, max_size);

	output.append(reinterpret_cast<char*>(&data_size), sizeof(data_size));
	output.append(&compressed[0], compression_size);

	/* hash the rsb1 and the rest of our bytecode, compared later in deserializer */

	std::uintptr_t first_hash = XXH32(&output[0], output.size(), kBytecodeHashSeed);
	std::uint8_t hashed_bytes[4]{};

	std::memcpy(hashed_bytes, &first_hash, sizeof(first_hash));

	for (std::size_t i = 0; i < output.size(); ++i)
		output[i] ^= (hashed_bytes[i % 4] + i * 41);

	return output;
}

std::string LuaSerializer::serialize(lua_State* L, const std::string& source, RbxOpEncoder encode, int ckey)
{
	std::ostringstream ss;

	// write basic data

	LoadS ls = { source.c_str(), source.size() };
	int error = lua_load(L, getS, &ls, "=");

	writeByte(ss, error);

	if (error != 0) {

		const char* err = lua_tostring(L, -1);
		size_t length = err ? strlen(err) : 0;

		writeInt(ss, length);
		ss.write(err, length);

		lua_pop(L, 1);
	}
	else {
		const LClosure* cl = static_cast<const LClosure*>(lua_topointer(L, -1));
		
		writeRoot(ss, cl->p, encode, ckey);
		lua_pop(L, 1);
	}

	std::string ssdata = ss.str();
	return compress(ssdata);
}

void LuaSerializer::writeRoot(std::ostringstream& ss, Proto* p, RbxOpEncoder encode, unsigned int ckey)
{
	unsigned int streamStart = ss.tellp();

	// marker for string table offset - will be patched later
	writeInt(ss, 0);

	std::map<const TString*, unsigned int> strings;

	// write prototype and fill string table
	writeProto(ss, strings, encode, p, ckey);

	// write string table
	unsigned int stringTableOffset = ss.tellp();

	std::vector<const TString*> stringTable(strings.size());
	
	for (std::map<const TString*, unsigned int>::const_iterator it = strings.begin(); it != strings.end(); ++it)
		stringTable[it->second - 1] = it->first;

	writeInt(ss, strings.size());

	for (size_t i = 0; i < stringTable.size(); ++i)
		writeInt(ss, stringTable[i]->tsv.len);

	for (size_t i = 0; i < stringTable.size(); ++i)
		ss.write(getstr(stringTable[i]), stringTable[i]->tsv.len);

	// patch string table offset
	ss.seekp(streamStart);
	writeInt(ss, stringTableOffset - streamStart);
}

void LuaSerializer::writeProto(std::ostringstream& ss, std::map<const TString*, unsigned int>& strings, RbxOpEncoder encode, Proto* p, unsigned int ckey)
{

	writeInt(ss, p->sizep);
	writeInt(ss, p->sizek);
	writeInt(ss, p->sizecode);
	writeInt(ss, p->sizelocvars);
	writeInt(ss, p->sizelineinfo);
	writeInt(ss, p->sizeupvalues);

	writeByte(ss, p->maxstacksize);
	writeByte(ss, p->is_vararg);
	writeByte(ss, p->nups);
	writeByte(ss, p->numparams);

	for (int i = 0; i < p->sizek; ++i) {
		const TValue* o = &p->k[i];

		switch (ttype(o)) {
		case LUA_TNIL: 
			writeByte(ss, Constant_Nil);
			break;
		case LUA_TBOOLEAN:
			writeByte(ss, bvalue(o) ? Constant_True : Constant_False); 
			break;
		case LUA_TNUMBER:
			writeByte(ss, Constant_Number);
			writeDouble(ss, nvalue(o));
			break;
		case LUA_TSTRING:
			writeByte(ss, Constant_String);
			writeString(ss, strings, rawtsvalue(o));
			break;
		default:
			lua_assert(0);
			break;
		}
	}

	int lastLine = 0;

	for (int i = 0; i < p->sizelineinfo; ++i) 
	{
		// the lines were previously encoded.
		writeInt(ss, p->lineinfo[i] - lastLine);

		lastLine = p->lineinfo[i];
	}

	for (int i = 0; i < p->sizelocvars; ++i)
	{
		const struct LocVar* l = &p->locvars[i];

		writeInt(ss, l->startpc);
		writeInt(ss, l->endpc);	
		writeString(ss, strings, l->varname);
	}

	for (int i = 0; i < p->sizeupvalues; ++i)
		writeString(ss, strings, p->upvalues[i]);
	
	for (int i = 0; i < p->sizecode; ++i) 
		writeInt(ss, encode(p->code[i], i, ckey));

	for (int i = 0; i < p->sizep; ++i)
		writeProto(ss, strings, encode, p->p[i], ckey);

}
