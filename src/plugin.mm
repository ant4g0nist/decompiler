#include <lldb/API/SBCommandInterpreter.h>
#include <lldb/API/SBCommandReturnObject.h>
#include <lldb/API/SBDebugger.h>
#include <lldb/API/SBFrame.h>
#include <lldb/API/SBProcess.h>
#include <lldb/API/SBThread.h>
#include <lldb/API/SBDefines.h>

#include <Foundation/Foundation.h>
#include <sstream>
#include <vector>
#include <string>

#include <retdec/retdec/retdec.h>
#include <retdec/config/config.h>
#include "plugin.h"

namespace lldb {
	bool PluginInitialize(lldb::SBDebugger debugger);
}

bool runDecompilation(retdec::config::Config& config, std::string* output = nullptr)
{
	try
	{
		auto rc = retdec::decompile(config, output);
		if (rc != 0)
		{
			throw std::runtime_error(
				"decompilation error code = " + std::to_string(rc)
			);
		}
	}
	catch (const std::runtime_error& e)
	{
		NSLog(@"Decompilation exception: %s", e.what());
		return true;
	}
	catch (...)
	{
		NSLog(@"Decompilation exception: unknown");
		return true;
	}

	return false;
}

void printCurrentFunctionPsuedoCode(lldb::SBTarget target, lldb::SBSymbol symbol, std::string* _decompiledCode) {
	
	NSString *decompiledString = [NSString stringWithUTF8String:_decompiledCode->c_str()];
	NSData *decompiledData = [decompiledString dataUsingEncoding:NSUTF8StringEncoding];

	NSError *error = nil;
    id jsonObject = [NSJSONSerialization JSONObjectWithData:decompiledData options:0 error:&error];
	if (error) {
        NSLog(@"Error parsing Decompiled Data %@", error.localizedDescription);
    }
    else
    {
		NSDictionary *decompiledJson = (NSDictionary *)jsonObject;
		NSArray* tokens = [decompiledJson  objectForKey:@"tokens"];
		NSMutableString* pseudoCode = [[NSMutableString alloc] init];
		lldb::addr_t start_address = symbol.GetStartAddress().GetFileAddress();
		lldb::addr_t end_address   = symbol.GetEndAddress().GetFileAddress();

		NSString * val = [NSString stringWithFormat:@"// Address range: 0x%llx - 0x%llx", start_address, end_address];
		bool start = false;

		for(NSDictionary* dict in tokens)
		{

			if([dict objectForKey:@"kind"]!=nil && [val isEqualToString:[dict objectForKey:@"val"]])
			{
				start = true;
			}


			if(start && [dict objectForKey:@"kind"]==nil && [[dict objectForKey:@"addr"] length]==0)
				break;

			else if(start)
			{
				NSString *val = [dict objectForKey:@"val"];
				if([val length])
					[pseudoCode appendString:val];
			}
		}

		NSLog(@"%@", pseudoCode);
    }
}

namespace lldb
{
	bool PluginInitialize(lldb::SBDebugger debugger);
}

class DecompileCommand: public lldb::SBCommandPluginInterface
{
private:
	bool fetched = false;
	retdec::config::Config config;

	virtual void fetchGlobalsAndFunctions(lldb::SBTarget target) {
		lldb::SBModule module = target.GetModuleAtIndex(0);

		int symNum = module.GetNumSymbols();
		for (int i = 0; i < symNum; ++i)
		{
			lldb::SBSymbol symbol = module.GetSymbolAtIndex(i);

			bool isImported = symbol.IsExternal();

			if (symbol.IsValid())
			{
				if (config.functions.getFunctionByStartAddress(symbol.GetStartAddress().GetFileAddress()) != nullptr)
				{
					continue;
				}

				if (symbol.GetType() == lldb::eSymbolTypeCode)
				{
					std::string fncName(symbol.GetName());
					retdec::common::Function ccFnc = retdec::common::Function(fncName);
					ccFnc.setStart(symbol.GetStartAddress().GetFileAddress());
					ccFnc.setEnd(symbol.GetEndAddress().GetFileAddress());
					
					config.functions.insert(ccFnc);
				}

				else if (symbol.GetType() == lldb::eSymbolTypeTrampoline)
				{
					retdec::common::Object var(symbol.GetName(), retdec::common::Storage::inMemory(symbol.GetStartAddress().GetFileAddress()));
					var.setRealName(symbol.GetName());
					config.globals.insert(var);
				}
			}
		}
	}

public:
	virtual bool DoExecute(lldb::SBDebugger debugger, char** command,
		lldb::SBCommandReturnObject& result)
	{
		lldb::SBTarget target = debugger.GetSelectedTarget();
		lldb::SBProcess process = target.GetProcess();
		lldb::SBFrame frame = process.GetSelectedThread().GetSelectedFrame();
		lldb::SBSymbol function = frame.GetSymbol();

		lldb::addr_t start_t= function.GetStartAddress().GetFileAddress();
		NSLog(@"Current function: %s %d", frame.GetFunctionName(), function.IsValid());

		config = retdec::config::Config::fromJsonString(DefaultConfigJSON);
		config.parameters.setInputFile(target.GetExecutable().GetFilename());
		// dummy parameters
		config.parameters.setOutputFormat("json");
		config.parameters.setOutputFile("hello.json");
		config.parameters.setIsVerboseOutput(false);

		if(!fetched)
			fetchGlobalsAndFunctions(target);

		std::string output;
		std::string* out = &output;

		if (runDecompilation(config, out))
		{
			NSLog(@"Failed to Decompile...%s", out);
			return false;
		}
		
		if (out == nullptr)
		{
			return false;
		}
		
		printCurrentFunctionPsuedoCode(target, function, out);

		return true;
	}
};

bool lldb::PluginInitialize(lldb::SBDebugger debugger)
{
	NSLog(@"Welcome to retdec lldb plugin");

	lldb::SBCommandInterpreter interpreter = debugger.GetCommandInterpreter();
	const char* decompile_show_help = "Decompile the current frame";
	lldb::SBCommand foo = interpreter.AddCommand("decompile", new DecompileCommand(), decompile_show_help);

	NSLog(@"New command added: decompile");

	return true;
}