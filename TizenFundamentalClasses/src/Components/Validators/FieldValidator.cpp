/*
 * FieldValidator.cpp
 *
 *  Created on: Nov 22, 2016
 *      Author: Kevin Winata
 */

#include "TFC/Components/Validators/FieldValidator.h"
#include <regex>
#include <sstream>

LIBAPI
TFC::Components::Validators::FieldValidator::FieldValidator(Field* field) :
	Validator(field),
	field(field)
{
}

bool TFC::Components::Validators::FieldValidator::ValidateInternal()
{
	if(field)
		return ValidateText(field->Text);

	return false;
}

LIBAPI
TFC::Components::Validators::LengthValidator::LengthValidator(Field* field, size_t min, size_t max) :
	FieldValidator(field),
	min(min),
	max(max)
{
}

bool TFC::Components::Validators::LengthValidator::ValidateText(std::string const& str)
{
	auto strlen = str.length();
	return (strlen >= min && strlen <= max);
}

LIBAPI
TFC::Components::Validators::NumberValidator::NumberValidator(Field* field) :
	FieldValidator(field)
{
}

bool TFC::Components::Validators::NumberValidator::ValidateText(std::string const& str)
{
	if (str.length() <= 0)
		return true;

	if (std::find_if_not(str.begin(), str.end(), (int(*)(int))std::isdigit) != str.end())
		return false;

	return true;
}

LIBAPI
TFC::Components::Validators::EmailValidator::EmailValidator(Field* field) :
	FieldValidator(field)
{
}

bool TFC::Components::Validators::EmailValidator::ValidateText(std::string const& str)
{
	if (str.length() <= 0)
		return true;

	std::regex email_regex("^[_A-Za-z0-9-\\+]+(\\.[_A-Za-z0-9-]+)*@[A-Za-z0-9-]+(\\.[A-Za-z0-9]+)*(\\.[A-Za-z]{2,})$");

	int res = std::regex_match(str, email_regex);
	if (res == 0)
		return false;

	return true;
}
