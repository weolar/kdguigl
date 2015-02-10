#include "stl/_locale.h"
#include "stl/_num_put.h"
#include "stl/_ctype.h"
#include "stl/_codecvt.h"

namespace std {

locale::id ctype<wchar_t>::id;
locale::id ctype<char>::id;
locale::id codecvt<wchar_t, char, mbstate_t>::id;

// typedef locale::id LocaleId;
// LocaleId ctype<char>::id;
// 
 void __stl_throw_length_error(const char* __msg) {
 	*(int*)0 = 0;
 }

void __stl_throw_out_of_range(const char* __msg) {
	*(int*)0 = 0;
}

// ios_base::ios_base() {
// 	*(int*)0 = 0;
// }
// 
// ios_base::~ios_base() {
// 	*(int*)0 = 0;
// }
// 
// void ios_base::_M_throw_failure () {
// 	*(int*)0 = 0;
// }
// 
// locale ios_base::imbue(const locale& loc) {
// 	*(int*)0 = 0;
// 	return locale();
// }
// //////////////////////////////////////////////////////////////////////////
// 
 locale::locale() {
 	*(int*)0 = 0;
 }

 locale::locale(std::locale const&) {
 	*(int*)0 = 0;
 }

 locale::~locale() {
 	*(int*)0 = 0;
 }

 const locale& locale::operator=(const locale& L) {
 	*(int*)0 = 0;
 	return *this;
 }

 locale::facet* locale::_M_use_facet(const locale::id& n) const {
 	return 0;
 }
namespace priv {

// locale::id& _GetFacetId(num_put<char, ostreambuf_iterator<char, char_traits<char> > > const*) {
// 	*(int*)0 = 0;
// 	return num_put<char, ostreambuf_iterator<char, char_traits<char> > >::id;
// }

} // priv

// bool uncaught_exception() {
// 	*(int*)0 = 0;
// }

} // std
