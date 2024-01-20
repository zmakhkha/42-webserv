import sys
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
from time import sleep

if __name__ == "__main__":
    options = Options()
    options.add_experimental_option("excludeSwitches", ["enable-logging"])
    driver = webdriver.Chrome(options=options)
    print("testing started")

    url = "http://localhost:9001/img.png"
    
    for _ in range(100):
        driver.execute_script("window.open('', '_blank');")
        driver.switch_to.window(driver.window_handles[-1])
        sleep(2)
        driver.get(url)

    sleep(50)
    driver.quit()
