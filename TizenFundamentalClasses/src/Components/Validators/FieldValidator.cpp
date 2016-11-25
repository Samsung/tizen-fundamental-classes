/*
 * FieldValidator.cpp
 *
 *  Created on: Nov 22, 2016
 *      Author: Kevin Winata
 */

#include "TFC/Components/Validators/FieldValidator.h"
#include <sstream>

using namespace TFC::Components::Validators;

LIBAPI
TFC::Components::Validators::FieldValidator::FieldValidator(Field* field) :
	Validator(field),
	field(field)
{
	errorDictionary[FieldValidator::ERROR_INVALID_COMPONENT] = "Field %0 is not valid / created yet.";
}

int TFC::Components::Validators::FieldValidator::ValidateInternal()
{
	if(field)
		return ValidateText(field->Text);

	return FieldValidator::ERROR_INVALID_COMPONENT;
}



int const LengthValidator::ERROR_LENGTH_LESS = 1;
int const LengthValidator::ERROR_LENGTH_MORE = 2;
LIBAPI
TFC::Components::Validators::LengthValidator::LengthValidator(Field* field, size_t min, size_t max) :
	FieldValidator(field),
	min(min),
	max(max)
{
	errorDictionary[LengthValidator::ERROR_LENGTH_LESS] = "Field %0 needs more than %1 characters.";
	errorDictionary[LengthValidator::ERROR_LENGTH_MORE] = "Field %0 cannot be more than %2 characters.";

	formatFunctions.push_back([&] () { return std::to_string(min).c_str(); });
	formatFunctions.push_back([&] () { return std::to_string(max).c_str(); });
}

int TFC::Components::Validators::LengthValidator::ValidateText(std::string const& str)
{
	auto strlen = str.length();
	if (strlen < min) return LengthValidator::ERROR_LENGTH_LESS;
	if (strlen > max) return LengthValidator::ERROR_LENGTH_MORE;
	return LengthValidator::ERROR_NONE;
}



int const NumberValidator::ERROR_NOT_NUMBER = 1;
LIBAPI
TFC::Components::Validators::NumberValidator::NumberValidator(Field* field) :
	FieldValidator(field)
{
	errorDictionary[NumberValidator::ERROR_NOT_NUMBER] = "Field %0 is not a number.";
}

int TFC::Components::Validators::NumberValidator::ValidateText(std::string const& str)
{
	if (!str.empty() && std::find_if_not(str.begin(), str.end(), (int(*)(int))std::isdigit) != str.end())
		return NumberValidator::ERROR_NOT_NUMBER;

	return NumberValidator::ERROR_NONE;
}



int const EmailValidator::ERROR_INVALID_EMAIL = 1;
LIBAPI
TFC::Components::Validators::EmailValidator::EmailValidator(Field* field) :
	FieldValidator(field)
{
	errorDictionary[EmailValidator::ERROR_INVALID_EMAIL] = "Field %0 is not a valid email.";
}

int TFC::Components::Validators::EmailValidator::ValidateText(std::string const& str)
{
	if (str.empty()) return EmailValidator::ERROR_NONE;

	std::regex email_regex("^[_A-Za-z0-9-\\+]+(\\.[_A-Za-z0-9-]+)*@[A-Za-z0-9-]+(\\.[A-Za-z0-9]+)*(\\.[A-Za-z]{2,})$");

	int res = std::regex_match(str, email_regex);
	if (res == 0)
		return EmailValidator::ERROR_INVALID_EMAIL;

	return EmailValidator::ERROR_NONE;
}
