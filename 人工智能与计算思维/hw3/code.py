from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from webdriver_manager.chrome import ChromeDriverManager
import time
import json
import csv

# ====================== 1. 首次登录 PubMed 并保存 Cookies ======================
def login_and_save_cookies(cookies_file="./data/mycookies.json"):
    """登录 PubMed 并保存 Cookies 到文件"""
    driver = webdriver.Chrome()
    driver.get("https://pubmed.ncbi.nlm.nih.gov/")

    time.sleep(20)

    # 等待登录成功（检查是否有登出按钮）
    try:
        WebDriverWait(driver, 10).until(
            EC.presence_of_element_located((By.CLASS_NAME, "account-logout"))
        )
        print("✅ 登录成功！")
    except:
        print("❌ 登录失败，请检查账号密码或验证码")
        driver.quit()
        return False

    # 获取 Cookies 并保存到文件
    cookies = driver.get_cookies()
    with open(cookies_file, "w") as f:
        json.dump(cookies, f)
    print(f"🔑 Cookies 已保存至 {cookies_file}")

    driver.quit()
    return True

# ====================== 2. 加载 Cookies 免登录访问 PubMed ======================
def load_cookies_and_search(cookies_file="./data/mycookies.json", keyword="cancer immunotherapy"):
    """加载 Cookies 并搜索 PubMed"""
    driver = webdriver.Chrome(ChromeDriverManager().install())
    driver.get("https://pubmed.ncbi.nlm.nih.gov/")  # 先访问首页

    # 加载 Cookies
    try:
        with open(cookies_file, "r") as f:
            cookies = json.load(f)
        for cookie in cookies:
            driver.add_cookie(cookie)
        print("🔑 Cookies 加载成功！")
    except:
        print("❌ 无法加载 Cookies，请先运行登录函数")
        driver.quit()
        return False

    # 刷新页面以应用 Cookies
    driver.refresh()

    # 检查是否登录成功
    try:
        WebDriverWait(driver, 5).until(
            EC.presence_of_element_located((By.CLASS_NAME, "account-logout"))
        )
        print("✅ 已自动登录 PubMed")
    except:
        print("❌ 登录失效，请重新获取 Cookies")
        driver.quit()
        return False

    # 搜索关键词
    search_box = driver.find_element(By.ID, "id_term")
    search_box.clear()
    search_box.send_keys(keyword)

    search_button = driver.find_element(By.CLASS_NAME, "search-btn")
    search_button.click()

    # 等待结果加载
    time.sleep(3)

    # 抓取论文信息
    articles = driver.find_elements(By.CLASS_NAME, "docsum-content")
    results = []

    for article in articles:
        try:
            title = article.find_element(By.CLASS_NAME, "docsum-title").text
        except:
            title = "N/A"
        
        try:
            abstract = article.find_element(By.CLASS_NAME, "full-view-snippet").text
        except:
            abstract = "N/A"
        
        try:
            journal = article.find_element(By.CLASS_NAME, "docsum-journal-citation").text
        except:
            journal = "N/A"
        
        results.append([title, abstract, journal])

    # 保存到 CSV
    with open('pubmed_results.csv', 'w', newline='', encoding='utf-8') as file:
        writer = csv.writer(file)
        writer.writerow(["Title", "Abstract", "Journal"])
        writer.writerows(results)
    
    print(f"📄 已保存 {len(results)} 篇论文到 pubmed_results.csv")
    driver.quit()

# ====================== 3. 主程序 ======================
if __name__ == "__main__":
    # 首次运行（登录并保存 Cookies）
    login_and_save_cookies()

    # 后续运行（直接加载 Cookies 免登录）
    load_cookies_and_search(keyword="machine learning")