local syntax = require "core.syntax"

syntax.add {
  files = "%.cs$",
  comment = "//",
  patterns = {
    { pattern = "//.-\n",                   type = "comment"  },
    { pattern = { "/%*", "%*/" },           type = "comment"  },
    { pattern = { '"', '"', '\\' },         type = "string"   },
    { pattern = { "[%$%@]?\"", '"', '\\' }, type = "string"   }, -- string interpolation and verbatim
    { pattern = "'\\x%x?%x?%x?%x'",         type = "string"   }, -- character hexadecimal escape sequence
    { pattern = "'\\u%x%x%x%x'",            type = "string"   }, -- character unicode escape sequence
    { pattern = "'\\?.'",                   type = "string"   }, -- character literal
    { pattern = "-?0x%x+",                  type = "number"   },
    { pattern = "-?%d+[%d%.eE]*f?",         type = "number"   },
    { pattern = "-?%.?%d+f?",               type = "number"   },
    { pattern = "[%+%-=/%*%^%%<>!~|&]",     type = "operator" },
    { pattern = "%?%?",                     type = "operator" }, -- ?? null-coalescing
    { pattern = "%?%.",                     type = "operator" }, -- ?. null-conditional
    { pattern = "[%a_][%w_]*%f[(]",         type = "function" },
    { pattern = "[%a_][%w_]*",              type = "symbol"   },
  },
  symbols = {
    -- keywords and contextual keywords
    ["abstract"]   = "keyword",
    ["as"]         = "keyword",
    ["add"]        = "keyword",
    ["await"]      = "keyword",
    ["base"]       = "keyword",
    ["break"]      = "keyword",
    ["case"]       = "keyword",
    ["catch"]      = "keyword",
    ["checked"]    = "keyword",
    ["class"]      = "keyword",
    ["record"]     = "keyword",
    ["const"]      = "keyword",
    ["continue"]   = "keyword",
    ["default"]    = "keyword",
    ["delegate"]   = "keyword",
    ["do"]         = "keyword",
    ["else"]       = "keyword",
    ["enum"]       = "keyword",
    ["event"]      = "keyword",
    ["explicit"]   = "keyword",
    ["extern"]     = "keyword",
    ["finally"]    = "keyword",
    ["fixed"]      = "keyword",
    ["for"]        = "keyword",
    ["foreach"]    = "keyword",
    ["get"]        = "keyword",
    ["goto"]       = "keyword",
    ["if"]         = "keyword",
    ["implicit"]   = "keyword",
    ["in"]         = "keyword",
    ["interface"]  = "keyword",
    ["internal"]   = "keyword",
    ["is"]         = "keyword",
    ["lock"]       = "keyword",
    ["namespace"]  = "keyword",
    ["new"]        = "keyword",
    ["operator"]   = "keyword",
    ["out"]        = "keyword",
    ["override"]   = "keyword",
    ["remove"]     = "keyword",
    ["params"]     = "keyword",
    ["partial"]    = "keyword",
    ["private"]    = "keyword",
    ["protected"]  = "keyword",
    ["dynamic"]    = "keyword",
    ["public"]     = "keyword",
    ["readonly"]   = "keyword",
    ["ref"]        = "keyword",
    ["return"]     = "keyword",
    ["sealed"]     = "keyword",
    ["set"]        = "keyword",
    ["sizeof"]     = "keyword",
    ["stackalloc"] = "keyword",
    ["static"]     = "keyword",
    ["struct"]     = "keyword",
    ["switch"]     = "keyword",
    ["this"]       = "keyword",
    ["throw"]      = "keyword",
    ["try"]        = "keyword",
    ["typeof"]     = "keyword",
    ["unchecked"]  = "keyword",
    ["unsafe"]     = "keyword",
    ["using"]      = "keyword",
    ["var"]        = "keyword",
    ["value"]      = "keyword",
    ["global"]     = "keyword",
    ["virtual"]    = "keyword",
    ["void"]       = "keyword",
    ["volatile"]   = "keyword",
    ["where"]      = "keyword",
    ["when"]       = "keyword",
    ["while"]      = "keyword",
    ["yield"]      = "keyword",
    -- types
    ["bool"]       = "keyword2",
    ["byte"]       = "keyword2",
    ["char"]       = "keyword2",
    ["decimal"]    = "keyword2",
    ["double"]     = "keyword2",
    ["float"]      = "keyword2",
    ["int"]        = "keyword2",
    ["long"]       = "keyword2",
    ["object"]     = "keyword2",
    ["sbyte"]      = "keyword2",
    ["short"]      = "keyword2",
    ["string"]     = "keyword2",
    ["uint"]       = "keyword2",
    ["ulong"]      = "keyword2",
    ["ushort"]     = "keyword2",
    -- literals
    ["true"]       = "literal",
    ["false"]      = "literal",
    ["null"]       = "literal",
  },
}
