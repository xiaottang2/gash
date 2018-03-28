/*
 * sym.cc -- Implementation of symbol related classes
 *
 * Author: Xiaoting Tang <tang_xiaoting@brown.edu>
 * Copyright: Xiaoting Tang (2018)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
  * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "sym.h"

namespace gashlang {


  /**
   * Symbol
   */
  NumSymbol::NumSymbol(string name, u32 version) : Symbol(name, version) {
    m_type = NUM;
  }

  NumSymbol::NumSymbol(NumSymbol& rhs) : Symbol(rhs.m_name, rhs.m_version) {
    m_bundle = Bundle(rhs.m_bundle.size());
  }

  ArraySymbol::ArraySymbol(string name, u32 version) : Symbol(name, version) {
    m_type = ARRAY;
  }

  ArraySymbol::ArraySymbol(ArraySymbol& rhs) : Symbol(rhs.m_name, rhs.m_version) {
    Bundle b;
    for (u32 i = 0; i < rhs.m_bundles.size(); ++i) {
      b = Bundle(rhs.m_bundles[i].size());
      m_bundles.push_back(b);
    }
  }

  FuncSymbol::FuncSymbol(string name, u32 version) : Symbol(name, version) {
    m_type = FUNC;
  }

  FuncSymbol::FuncSymbol(FuncSymbol& rhs) : Symbol(rhs.m_name, rhs.m_version) {
    m_func = rhs.m_func;
  }

  /**
   * Symbol Store
   */

  bool SymbolStore::has_symbol_for_name(string name) {
    return m_symbols.find(name) != m_symbols.end();
  }

  void SymbolStore::require_has_symbol_for_name(string name) {
    if (!has_symbol_for_name(name))
      FATAL("Does not has symbol " << name );
  }

  bool SymbolStore::version_consistency_check() {
    for (auto it = m_symbols.begin(); it != m_symbols.end(); ++it) {
      u32 version = 0;
      for (auto jt = *it.begin(); jt != *it.end(); ++jt) {
        if (*jt->m_version != version++)
          return false;
      }
    }
    return true;
  }

  bool SymbolStore::version_consistency_check_for_name(string name) {
    require_has_symbol_for_name(name);
    auto it = this->m_symbols.find(name);
    u32 version = 0;
    for (auto jt = *it.begin(); jt != *it.end(); ++jt) {
      if (*jt->m_version != version++)
        return false;
    }
  }

  u32 SymbolStore::get_newest_version_for_name(string name) {
    require_has_symbol_for_name(name);
    return m_symbols.find(name)->second->back()->m_id;
  }

  Symbol* SymbolStore::get_symbol_for_name(string name) {
    require_has_symbol_for_name(name);
    return m_symbols.find(name)->second()->back();
  }

  Symbol* SymbolStore::new_symbol(string name, SymbolType type) {
    Symbol* sym;
    if (has_symbol_for_name(name)) {
      Symbol* sym_old = get_symbol_for_name(name);
      if (type != sym_old->m_type) {
        FATAL("Symbol " << name << " already has type " << type <<
              ", but now you want a new symbol with the same name of type " << type);
      }
      switch(type) {
      case NUM:
        sym = (Symbol*) new NumSymbol(*static_cast<NumSymbol*>(sym_old));
        break;
      case ARRAY:
        sym = (Symbol*) new ArraySymbol(*static_cast<ArraySymbol*>(sym_old));
        break;
      case FUNC:
        sym = (Symbol*) new FuncSymbol(*static_cast<FuncSymbol*>(sym_old));
        break;
      }
      sym->gets_older();
      m_symbols.find(sym->m_name)->second.push_back(sym);

    } else {
      // Creating a new list of symbols associated with the name, starting from version 0.
      switch(type) {
      case NUM:
        sym = (Symbol*) new NumSymbol(name, 0);
        break;
      case ARRAY:
        sym = (Symbol*) new ArraySymbol(name, 0);
        break;
      case FUNC:
        sym = (Symbol*) new FuncSymbol(name, 0);
        break;
      }
      vector<Symbol*> symbol_list({sym});
      m_symbols.insert(make_pair(name, symbol_list));
    }
  }


}  // gashlang
