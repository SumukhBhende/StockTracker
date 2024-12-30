from datetime import datetime
import pandas as pd
from datetime import date
import requests
from bs4 import BeautifulSoup
import re

def totalv():
    # Read the Excel file
    stocks_today_df = pd.read_excel('Stocks Today.xlsx')

    # Calculate the sum of the "Total Value" column
    total_value_sum = stocks_today_df['Total Value'].sum()

    # Create a new row with the total value
    total_row = pd.DataFrame([{'Stocks': '', 'Quantity': '', 'Stock Prices': 'total =', 'Total Value': total_value_sum, 'Closed Price Date': ''}])

    # Append the total value row to the DataFrame
    stocks_today_df = pd.concat([stocks_today_df, total_row], ignore_index=True)

    # Save the updated DataFrame back to the Excel file
    stocks_today_df.to_excel('Stocks Today.xlsx', index=False)

def add_closed_price_date():
    # Read the Excel file
    stocks_today_df = pd.read_excel('Stocks Today.xlsx')

    # Filter out the extra row
    stocks_today_df = stocks_today_df[stocks_today_df['Stock Prices'] != 'total =']

    # Get today's date
    today_date = date.today()

    # Convert the 'Date' column to datetime.date type
    stocks_today_df['Date'] = pd.to_datetime(stocks_today_df['Date']).dt.date

    # Check if the date under the "Date" column matches today's date
    date_match = stocks_today_df['Date'].eq(today_date).any()

    if not date_match:
        stock_names = stocks_today_df['Stocks'].tolist()
        stock_store_df = pd.read_excel('StockStore.xlsx')
        stock_symbols = stock_store_df[stock_store_df['Stocks'].isin(stock_names)]['Symbols'].tolist()
        closed_price_dates = []

        for symbol in stock_symbols:
            url = f'https://www.google.com/finance/quote/{symbol}:NSE'
            response = requests.get(url)
            soup = BeautifulSoup(response.text, 'html.parser')
            
            # Find the closed price date on the page
            date_span = soup.find('div', class_='ygUjEc')
            if date_span:
                closed_price_date = date_span.text
                closed_price_dates.append(closed_price_date)
            else:
                closed_price_dates.append(None)

        # Add the "Closed Price Date" column to the stocks_today_df DataFrame
        stocks_today_df['Closed Price Date'] = closed_price_dates

        # Save the updated DataFrame back to the Excel file
        stocks_today_df.to_excel('Stocks Today.xlsx', index=False)

def stockupdate():

    # Read the Excel file
    stocks_today_df = pd.read_excel('Stocks Today.xlsx')

    # Get today's date
    today_date = date.today()

    # Convert the 'Date' column to datetime.date type
    stocks_today_df['Date'] = pd.to_datetime(stocks_today_df['Date']).dt.date

    # Check if the date under the "Date" column matches today's date
    date_match = stocks_today_df['Date'].eq(today_date).any()

    if not date_match:
        stock_names = stocks_today_df['Stocks'].tolist()
        stock_store_df = pd.read_excel('StockStore.xlsx')
        stock_symbols = stock_store_df[stock_store_df['Stocks'].isin(stock_names)]['Symbols'].tolist()
        stock_prices = []

        for symbol in stock_symbols:
            url = f'https://www.google.com/finance/quote/{symbol}:NSE'
            response = requests.get(url)
            soup = BeautifulSoup(response.text, 'html.parser')
            
            # Find the stock price on the page
            price_span = soup.find('div', class_='YMlKec fxKbKc')
            if price_span:
                price_text = price_span.text
                price_text = re.sub(r'[^\d.]', '', price_text)  # Remove non-numeric characters
                try:
                    price = float(price_text)
                    stock_prices.append(price)
                except ValueError:
                    stock_prices.append(None)
            else:
                stock_prices.append(None)

        # Update the "Stock Prices" column in the stocks_today_df DataFrame
        stocks_today_df.loc[~stocks_today_df['Date'].eq(today_date), 'Stock Prices'] = stock_prices

        # Calculate the total value and update the "Total Value" column
        stocks_today_df['Total Value'] = stocks_today_df['Stock Prices'] * stocks_today_df['Quantity']

        # Update the first row of the date column to today's date, leave the rest blank
        stocks_today_df.at[0, 'Date'] = today_date

        # Save the updated DataFrame back to the Excel file
        stocks_today_df.to_excel('Stocks Today.xlsx', index=False)


def timecheck():
    current_time = datetime.now().time()
    return current_time

# Get the current system time
i=0

# Check if the current time is past 4 PM (16:00:00)
while i==0:
    current_time = timecheck()
    if current_time >= datetime.strptime('16:00:00', '%H:%M:%S').time():
        stockupdate()
        add_closed_price_date()
        totalv()
        i=1
    else:
        continue
