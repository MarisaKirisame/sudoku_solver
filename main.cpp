#include <array>
#include <../CSP/CSP.hpp>
#include <vector>
#include <iterator>
#include <../cpp_common/iterator.hpp>
#include <iostream>
#include <string>
int main( )
{
    using VARIABLE_ID_T = std::pair< size_t, size_t >;
    std::map< VARIABLE_ID_T, std::list< size_t > > var;
    std::list< CSP::local_constraint< VARIABLE_ID_T, size_t > > con;
    auto make_difference =
        [&]( VARIABLE_ID_T x, VARIABLE_ID_T y )
        { con.push_back( CSP::make_local_constraint< VARIABLE_ID_T, size_t >(
            { x, y },
            []( const auto & v ){ return v[0].get( ) != v[1].get( ); } ) ); };
    std::string parse;
    std::cin >> parse;
    for ( size_t i = 0; i < 9; ++i )
    {
        for ( size_t j = 0; j < 9; ++j )
        {
            var.insert( {
                { i, j },
                parse[ i * 9 + j ] == '0' ?
                    std::list< size_t >{ 1, 2, 3, 4, 5, 6, 7, 8, 9 } :
                    std::list< size_t >{ static_cast< size_t >( parse[ i * 9 + j ] - '0' ) } } );
            for ( size_t k = i + 1; k < 9; ++k ) { make_difference( { i, j }, { k, j } ); }
            for ( size_t k = j + 1; k < 9; ++k ) { make_difference( { i, j }, { i, k } ); }
            for ( size_t k = i; k / 3 == i / 3; ++k )
            { for ( size_t l = j; l / 3 == j / 3; ++l ) { if ( ! ( k == i && l == j ) ) { make_difference( { i, j }, { k, l } ); } } }
        }
    }
    CSP::backtracking_search(
        var,
        2,
        con,
        common::make_function_output_iterator(
            []( const auto & m )
            {
                for ( size_t i = 0; i < 9; ++i )
                {
                    for ( size_t j = 0; j < 9; j++ ) { std::cout << m.find( { i, j } )->second; }
                    std::cout << std::endl;
                }
                std::cout << std::endl;
            } ) );
}
