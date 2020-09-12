#include "goalc/compiler/Compiler.h"

MathMode Compiler::get_math_mode(const TypeSpec& ts) {
  if (m_ts.typecheck(m_ts.make_typespec("binteger"), ts, "", false, false)) {
    return MATH_BINT;
  }

  if (m_ts.typecheck(m_ts.make_typespec("integer"), ts, "", false, false)) {
    return MATH_INT;
  }

  if (m_ts.typecheck(m_ts.make_typespec("float"), ts, "", false, false)) {
    return MATH_FLOAT;
  }

  return MATH_INVALID;
}

bool Compiler::is_number(const TypeSpec& ts) {
  return m_ts.typecheck(m_ts.make_typespec("number"), ts, "", false, false);
}

bool Compiler::is_float(const TypeSpec& ts) {
  return m_ts.typecheck(m_ts.make_typespec("float"), ts, "", false, false);
}

bool Compiler::is_integer(const TypeSpec& ts) {
  return m_ts.typecheck(m_ts.make_typespec("integer"), ts, "", false, false) &&
         !m_ts.typecheck(m_ts.make_typespec("binteger"), ts, "", false, false);
}

bool Compiler::is_binteger(const TypeSpec& ts) {
  return m_ts.typecheck(m_ts.make_typespec("binteger"), ts, "", false, false);
}

bool Compiler::is_singed_integer_or_binteger(const TypeSpec& ts) {
  return m_ts.typecheck(m_ts.make_typespec("integer"), ts, "", false, false) &&
         !m_ts.typecheck(m_ts.make_typespec("uinteger"), ts, "", false, false);
}

Val* Compiler::number_to_integer(Val* in, Env* env) {
  auto ts = in->type();
  if (is_binteger(ts)) {
    assert(false);
  } else if (is_float(ts)) {
    assert(false);
  } else if (is_integer(ts)) {
    return in;
  } else {
    assert(false);
  }
}

Val* Compiler::number_to_binteger(Val* in, Env* env) {
  auto ts = in->type();
  if (is_binteger(ts)) {
    return in;
  } else if (is_float(ts)) {
    assert(false);
  } else if (is_integer(ts)) {
    assert(false);
  } else {
    assert(false);
  }
}

Val* Compiler::number_to_float(Val* in, Env* env) {
  auto ts = in->type();
  if (is_binteger(ts)) {
    assert(false);
  } else if (is_float(ts)) {
    return in;
  } else if (is_integer(ts)) {
    assert(false);
  } else {
    assert(false);
  }
}

Val* Compiler::to_math_type(Val* in, MathMode mode, Env* env) {
  switch (mode) {
    case MATH_BINT:
      return number_to_binteger(in, env);
    case MATH_INT:
      return number_to_integer(in, env);
    case MATH_FLOAT:
      return number_to_float(in, env);
    default:
      assert(false);
  }
}

Val* Compiler::compile_add(const goos::Object& form, const goos::Object& rest, Env* env) {
  auto args = get_va(form, rest);
  if (!args.named.empty() || args.unnamed.empty()) {
    throw_compile_error(form, "Invalid + form");
  }

  // compile values
  std::vector<Val*> values;
  for (auto& x : args.unnamed) {
    values.push_back(compile_error_guard(x, env));
  }

  // look at the first value to determine the math mode
  auto first_type = values.at(0)->type();
  auto math_type = get_math_mode(first_type);
  switch (math_type) {
    case MATH_INT: {
      auto result = env->make_gpr(first_type);
      env->emit(std::make_unique<IR_RegSet>(result, values.at(0)->to_gpr(env)));

      for (size_t i = 1; i < values.size(); i++) {
        env->emit(std::make_unique<IR_IntegerMath>(IntegerMathKind::ADD_64, result,
                                                   values.at(i)->to_gpr(env)));
      }
      return result;
    }
    case MATH_INVALID:
      throw_compile_error(
          form, "Cannot determine the math mode for object of type " + first_type.print());
      break;
    default:
      assert(false);
  }
  assert(false);
  return get_none();
}