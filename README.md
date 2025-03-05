# About

This plugin is an extension for the Unreal Editor (version 5.3 or newer) to provide convenience features missing from the translation editor in the localization dashboard.
With this first release the plugin adds the following feature to the translation editor:
  * Toggle All Reviewed - This is a single button which toggles all entries to review on or off (e.g. after importing translations from a .po file).
  * Transfer Source - This is a single button which copies the value from the source to translation column (in completed and untranslated tabs)
  * Clear Translation - This is a single button which clear the translation column (in completed and untranslated tabs)
 

# How to Use

This plugin is provided as source code and requires you to build it with your Unreal Engine project.
  * Download the repository and copy the content into a folder within the `Plugins` folder of your Unreal Engine project.
  * Open your project and let it rebuild the plugin.
  * For C++ projects, regenerate your solution file, open it in your IDE and build your project.

# Remarks

You may need to check if your translation editor and desired tab to work on is in user focus. The Unreal Editor UI uses a slight blue glow on tabs to indicate that (or what I assume is user focus).  
The following snippets illustrate the difference. Click on or inside the tab's body to give focus.

|![image](https://github.com/user-attachments/assets/af30c69b-d949-4166-9f80-2dbde3efc4bc) |  ![image](https://github.com/user-attachments/assets/97741bb4-8b08-4d34-ae8e-78515c7868da)|
| ----------- | ----------- |
| *No focus at all* | *Need Review is in focus* |
