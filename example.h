#ifndef EXAMPLE_H
#define EXAMPLE_H
#include <string>
#include <iostream>
#include <set>
#include <array>
#include <boost/multi_array.hpp>
#include <utility>
#include <algorithm>
#include <boost/iterator/counting_iterator.hpp>
namespace sudoku_solver
{
    constexpr size_t associated_squares_count = 20;
    struct sudoku
    {
        typedef std::pair< size_t, size_t > index;
        struct square
        {
            std::set< size_t > possible_number = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
            index this_index;
        };
        boost::multi_array< square, 2 > squares;
        sudoku( ) : squares( boost::extents[9][9] )
        {
            for ( size_t i = 0; i < 9; ++i )
            {
                for ( size_t j = 0; j < 9; ++j )
                { squares[i][j].this_index = index( i, j ); }
            }
        }
        template< typename OUTITER >
        OUTITER all_associated( const index & in, OUTITER result ) const
        {
            auto sent = [&]( const index & i )
            {
                if ( i != in )
                {
                    *result = i;
                    ++result;
                }
            };
            size_t small_squares_x_index = in.first - in.first % 3;
            size_t small_squares_y_index = in.second - in.second % 3;
            std::for_each( boost::counting_iterator< size_t >( 0 ),
                           boost::counting_iterator< size_t >( 9 ),
                           [&]( size_t s )
            {
                sent( index( in.first, s ) );
                sent( index( s, in.second ) );
                auto x = small_squares_x_index + s % 3;
                auto y = small_squares_y_index + ( s - s % 3 ) / 3;
                if ( x != in.first && y != in.second ) { sent( index( x, y ) ); }
            } );
            return result;
        }
        void fill( const index & in, size_t num )
        {
            if( squares[in.first][in.second].possible_number.count( num ) != 0 )
            {
                squares[in.first][in.second].possible_number = { num };
                notify_determined( in );
            }
            else
            { squares[in.first][in.second].possible_number = { }; }
        }
        void notify_determined( const index & in )
        {
            assert( squares[in.first][in.second].possible_number.size( ) == 1 );
            size_t num = * squares[in.first][in.second].possible_number.begin( );
            std::vector< index > ass;
            ass.reserve( associated_squares_count );
            all_associated( in, std::back_inserter< decltype( ass ) >( ass ) );
            std::for_each( ass.begin( ),
                           ass.end( ),
                           [&]( const index & i )
            {
                auto & square = squares[i.first][i.second].possible_number;
                if ( square.count( num ) == 1 )
                {
                    square.erase( num );
                    if ( square.size( ) == 1 )
                    { notify_determined( i ); }
                }
            } );
        }
        struct completed_sudoku
        {
            boost::multi_array< size_t, 2 > squares = boost::multi_array< size_t, 2 >( boost::extents[9][9] );
            friend std::ostream & operator << ( std::ostream & os, const completed_sudoku & cs )
            {
                for ( size_t j = 0; j < 9; ++j )
                {
                    for ( size_t i = 0; i < 9; ++i )
                    {
                        os << cs.squares[i][j];
                    }
                    os << std::endl;
                }
                return os;
            }
        };
        operator completed_sudoku( ) const
        {
            check_valid( );
            completed_sudoku ret;
            for ( size_t i = 0; i < 9; ++i )
            {
                for ( size_t j = 0; j < 9; ++j )
                {
                    assert( squares[i][j].possible_number.size( ) == 1 );
                    ret.squares[i][j] = * squares[i][j].possible_number.begin( );
                }
            }
            return ret;
        }
        void check_valid( ) const
        {
            std::for_each( squares.origin( ),
                           squares.origin( ) + squares.num_elements( ),
                           [&]( const square & s )
            {
                if ( s.possible_number.size( ) != 1 ) { return; }
                std::vector< index > ass;
                ass.reserve( associated_squares_count );
                all_associated( s.this_index, std::back_inserter< decltype( ass ) >( ass ) );
                std::for_each( ass.begin( ),
                               ass.end( ),
                               [&]( const index & i )
                { assert( squares[i.first][i.second].possible_number != s.possible_number ); } );
            } );
        }
        template< typename OUTITER >
        OUTITER solve( OUTITER result )
        {
            if ( std::any_of( squares.origin( ),
                              squares.origin( ) + squares.num_elements( ),
                              []( const square & s ){ return s.possible_number.empty( ); } ) ) { return result; }
            if ( std::none_of( squares.origin( ),
                               squares.origin( ) + squares.num_elements( ),
                               []( const square & s ){ return s.possible_number.size( ) != 1; } ) )
            {
                *result = static_cast< completed_sudoku >( *this );
                ++result;
                return result;
            }
            square try_square = *std::min_element( squares.origin( ), squares.origin( ) + squares.num_elements( ), []( const square & l, const square & r )
            {
                if ( l.possible_number.size( ) == 1 ) { return false; }
                if ( r.possible_number.size( ) == 1 ) { return true; }
                return l.possible_number.size( ) < r.possible_number.size( );
            } );
            size_t try_num = * try_square.possible_number.begin( );
            assert( try_square.possible_number.size( ) > 1 );
            sudoku rest( * this );
            rest.squares[try_square.this_index.first][try_square.this_index.second].possible_number.erase( try_num );
            if ( rest.squares[try_square.this_index.first][try_square.this_index.second].possible_number.size( ) == 1 )
            { rest.notify_determined( try_square.this_index ); }
            fill( try_square.this_index, try_num );
            result = solve( result );
            return rest.solve( result );
        }
        void prase( const std::string & str )
        {
            for ( size_t j = 0; j < 9; ++j )
            {
                for ( size_t i = 0; i < 9; ++i )
                {
                    const char & c = str[j*9+i];
                    if ( c != '0' ) { fill( index( i, j ), c - '0' ); }
                }
            }

        }
    };

    void example( )
    {
        sudoku su;
        std::string str;
        std::cin >> str;
        su.prase( str );
        su.solve( std::ostream_iterator< sudoku::completed_sudoku >( std::cout, "\n" ) );
    }
}
#endif // EXAMPLE_H
